#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring> // Used for memset
#include <errno.h>

#include "SocketFactory.h"
#include "UserInput.h"

int SocketFactory::createServerSocket(struct sockaddr_in& server_addr, int& socketType) {
	int sockfd = socket(server_addr.sin_family, socketType, 0);
	if (sockfd < 0) {
		logger_->error_log("SocketFactory: socket failed (" + std::to_string(errno) + ")");
		return -1;
	}

	auto sin = reinterpret_cast<sockaddr*>(&server_addr);
	int ret = bind(sockfd, sin, sizeof(*sin));
	if (ret < 0) {
		logger_->error_log("SocketFactory: bind failed (" + std::to_string(errno) + ")");
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

int SocketFactory::createClientSocket(struct sockaddr_in& server_addr, int& socketType) {
	return -1;
};

int SocketFactory::createSocket() {
	struct sockaddr_in server_addr;
	int socketType;

	if (!cParse_.getConfigs(server_addr, socketType) && !getConfigFromUser(server_addr, socketType)){
		logger_->error_log("SocketFactory: failed to find or create configs");
		return -1;
	}


	if (entType_ == EntityType::Server) {
		logger_->info_log("SocketFactory: creating server sockets");
		return createServerSocket(server_addr, socketType);
	}

	return createClientSocket(server_addr, socketType);
};

bool SocketFactory::acceptConnection(int& sockfd) {
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	int con = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (con < 0) {
		logger_->error_log("SocketFactory: accept failed (" + std::to_string(errno) + ")");
		return false;
	}

	return true;
};

int SocketFactory::closeSocket(int& sock) {
	return close(sock);
};

bool SocketFactory::getConfigFromUser(struct sockaddr_in& server_addr, int& socketType) {
	UserInput ui;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = ui.requestOption(ConfigParser::FamilyOptionMap);   //AF_LOCAL;
	inet_aton(ui.requestString("IP Address:").data(), &server_addr.sin_addr); //net_addr(argv[1]);
	server_addr.sin_port = htons(ui.requestInt("Port:")); //htons(8000);
	socketType = ui.requestOption(ConfigParser::SockTypeMap);
	return true;
}