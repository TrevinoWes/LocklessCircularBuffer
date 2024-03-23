#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "SocketFactory.h"
#include "UserInput.h"

namespace Network {

int SocketFactory::createServerSocket(NetworkSettings& netSet) {
	int sockfd = socket(netSet.server_addr.sin_family, netSet.sockType, 0);
	if (sockfd < 0) {
		logger_->error_log("SocketFactory: socket failed (" + std::string(std::strerror(errno)) + ")");
		return -1;
	}

	auto sin = reinterpret_cast<sockaddr*>(&netSet.server_addr);
	if(bind(sockfd, sin, sizeof(*sin)) < 0) {
		logger_->error_log("SocketFactory: bind failed (" + std::string(std::strerror(errno)) + ")");
		closeSocket(sockfd);
		return -1;
	}

	netSet.server_addr.sin_port = netSet.client_port;
	if (connect(sockfd, sin, sizeof(*sin)) < 0) {
		logger_->error_log("SocketFactory: connect failed (" + std::to_string(errno) + ") " + std::string(std::strerror(errno)));
		closeSocket(sockfd);
		return -1;
	}

/*
	if (listen(sockfd, 1) < 0) {
		logger_->error_log("SocketFactory: listen failed");
		closeSocket(sockfd);
		return -1;
	}
*/
	return sockfd;
};

int SocketFactory::createClientSocket(NetworkSettings& netSet) {
	int sockfd = socket(netSet.server_addr.sin_family, netSet.sockType, 0);
	if (sockfd < 0) {
		logger_->error_log("SocketFactory: socket failed (" + std::string(std::strerror(errno)) + ")");
		return -1;
	}

	auto sin = reinterpret_cast<sockaddr*>(&netSet.server_addr);
	netSet.server_addr.sin_port = netSet.client_port;
	if(bind(sockfd, sin, sizeof(*sin)) < 0) {
		logger_->error_log("SocketFactory: bind failed (" + std::string(std::strerror(errno)) + ")");
		closeSocket(sockfd);
		return -1;
	}	


	netSet.server_addr.sin_port = netSet.server_port;
	if (connect(sockfd, sin, sizeof(*sin)) < 0) {
		logger_->error_log("SocketFactory: connect failed (" + std::string(std::strerror(errno)) + ")");
		closeSocket(sockfd);
		return -1;
	}

	return sockfd;
};

int SocketFactory::createSocket() {
	NetworkSettings netSet;

	if (!cParse_.getConfigs(netSet) && !getConfigFromUser(netSet)){
		logger_->error_log("SocketFactory: failed to find or create configs");
		return -1;
	}

	// TODO: Remove when other configurations are supported
	if(netSet.sockType != ConfigParser::SockTypeMap.at("SOCK_DGRAM") && 
	   netSet.server_addr.sin_family != ConfigParser::FamilyOptionMap.at("AF_INET")) {
		return -1;
	}


	if (entType_ == EntityType::Server) {
		return createServerSocket(netSet);
	}

	return createClientSocket(netSet);
};

int SocketFactory::closeSocket(int& sock) {
	return close(sock);
};

bool SocketFactory::getConfigFromUser(NetworkSettings& netSet) {
	UserInput ui;

	netSet.server_addr = {};
	netSet.server_addr.sin_family = ui.requestOption(ConfigParser::FamilyOptionMap);
	inet_aton(ui.requestString("IP Address:").data(), &netSet.server_addr.sin_addr);
	netSet.server_port = htons(ui.requestInt("Server Port:"));
	netSet.server_addr.sin_port = netSet.server_port;
	netSet.client_port= htons(ui.requestInt("Client Port:"));
	netSet.sockType = ui.requestOption(ConfigParser::SockTypeMap);
	return true;
}
} //Network