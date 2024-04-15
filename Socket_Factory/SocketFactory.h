#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include "Logger.h"
#include "ConfigParser.h"
#include "SharedTypes.h"

namespace Network {

class SocketFactory {
private:
	ConfigParser cParse_;
	EntityType entType_;
	static const std::map<std::string, int> FamilyOptionMap;
	static const std::map<std::string, int> SockTypeMap;
	std::shared_ptr<Logger> logger_;
	uint32_t iovcnt_;
	struct iovec* iov;

	int createServerSocket(int& sockfd, NetworkSettings& netSet);
	int createClientSocket(int& sockfd, NetworkSettings& netSet);
	bool getConfigFromUser(NetworkSettings& netSet);
public:

	SocketFactory(EntityType type, std::string name, const int& iovcnt = 0);
	/*
	TODO:
	define copy constructor
	copy assignment
	move constructor
	move assignment
	*/
	~SocketFactory();
	int createSocket(const int& msTimeout = 0);
	int closeSocket(int& socket);

	// Return Value:
	// sentinel value 0 means socket is empty
	template <class t>
	int readVal(int& sockfd, t& val, const int& flags) {
		unsigned char* buffer = reinterpret_cast<unsigned char*>(&val);
		int size = sizeof(val), ret = 0;
		// Should set MSG_DONTWAIT on the FD instead of managing per recv call
		
		while(size > 0) {
			ret = ::recv(sockfd, buffer, size, flags);
			if(ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				ret = 0;
				break;
			} else if (ret < 0) {
				logger_->error_log("Read failed (" + std::string(std::strerror(errno)) + ")");
				break;
			}
			size -= ret;
			buffer += ret;
		}

		if(ret > 0) {
			val = ntohl(val); // ntohl is not dynamic based on type
		}

		return ret;
	}

	template <class t>
	int readValues(int& sockfd, std::vector<t>& buffer) {
		if(iovcnt_ == 0) {
			logger_->error_log("Read failed, iovcnt_ is not initialized");
			return -1;
		}

		if(iovcnt_ != buffer.size()) {
			logger_->error_log("Read failed, buffer incorrect size");
			return -1;
		}

		if(iov[iovcnt_ - 1].iov_base != &buffer.back()) {
			for(uint32_t i = 0; i < iovcnt_; ++i) {
				iov[i].iov_base = &buffer[i];
				iov[i].iov_len = sizeof(t);
			}
		}

		auto ret = ::readv(sockfd, iov, iovcnt_);
		if(ret < 0  && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			ret = 0;
		} else if(ret < 0) {
			logger_->error_log("read failed (" + std::string(std::strerror(errno)) + ")");
			return ret;
		}

		auto buffSize = ret / sizeof(t);
		for(uint32_t i = 0; i < buffSize; ++i) {
			buffer[i] = ntohl(buffer[i]); // not dynamic based on type
		}

		return buffSize;
	};

	template <class t>
	int writeVal(int& sockfd, t val, const int& flags) {
		unsigned char* buffer = reinterpret_cast<unsigned char*>(&val);
		int size = sizeof(val), ret = 0;
		val = htonl(val); // htonl is not dynamic 

		while(size > 0) {
			ret = ::send(sockfd, buffer, size, flags);
			if (ret < 0) {
				logger_->error_log("write failed (" + std::string(std::strerror(errno)) + ")");
				break;
			}

			size -= ret;
			buffer += ret;
		}

		return ret;
	}

	// !!! flimsy implementation between buffer size and iov struct array size
	template <class t>
	int writeValues(int& sockfd, std::vector<t> buffer) {
		if(iovcnt_ == 0) {
			logger_->error_log("Read failed, iovcnt_ is not initialized");
			return -1;
		}

		if(iovcnt_ != buffer.size()) {
			logger_->error_log("Read failed, buffer incorrect size");
			return -1;
		}

		if(iov[iovcnt_ - 1].iov_base != &buffer.back()) {
			for(uint32_t i = 0; i < iovcnt_; ++i) {
				iov[i].iov_base = &buffer[i];
				iov[i].iov_len = sizeof(t);
			}
		}

		for(uint32_t i = 0; i < buffer.size(); ++i) {
			buffer[i] = htonl(buffer[i]); // not dynamic depending on sent type
		}

		auto size = iovcnt_ * sizeof(t);
		auto offset = 0;
		while(size > 0) {
			auto ret = ::writev(sockfd, iov + offset, iovcnt_ - offset);
			if(ret < 0  && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				ret = 0;
			} else if(ret < 0) {
				logger_->error_log("read failed (" + std::string(std::strerror(errno)) + ")");
				return ret;
			}

			size -= ret;
			offset += ret / sizeof(t);
		}

		return offset;
	};

};
} //Network


/*
size = 10 * 4 = 40
offset = 0

01 | 23456789

size = 32
offset = 8
iov + 2
iovcnt_ = 10 - (32 / 12)

*/