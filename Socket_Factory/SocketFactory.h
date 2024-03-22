#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <cstring>
#include <errno.h>

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

	int createServerSocket(NetworkSettings& netSet);
	int createClientSocket(NetworkSettings& netSet);
	bool getConfigFromUser(NetworkSettings& netSet);
public:

	SocketFactory(EntityType type, std::string name): cParse_(type, name), entType_(type), logger_(Logger::getLogger()) {};
	int createSocket();
	int closeSocket(int& socket);

	// Return Value:
	// sentinel value 0 means socket is empty
	template <class t>
	int readVal(int& sockfd, t& val, int& flags) {
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
	int sendVal(int& sockfd, t val) {
		unsigned char* buffer = reinterpret_cast<unsigned char*>(&val);
		int size = sizeof(val), ret = 0;
		int flags = 0;
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
};
} //Network