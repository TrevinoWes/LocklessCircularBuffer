#pragma once

#include <map>
#include <string>
#include <netinet/in.h>

#include "Logger.h"
#include "ConfigParser.h"
#include "SharedTypes.h"

class SocketFactory {
private:
	ConfigParser cParse_;
	EntityType entType_;
	static const std::map<std::string, int> FamilyOptionMap;
	static const std::map<std::string, int> SockTypeMap;
	std::shared_ptr<Logger> logger_;

	int createServerSocket(struct sockaddr_in& server_addr, int& socketType);
	int createClientSocket(struct sockaddr_in& server_addr, int& socketType);
	bool getConfigFromUser(struct sockaddr_in& server_addr, int& socketType);
public:
	//using EntityType = EntityType;

	SocketFactory(EntityType type, std::string name): cParse_(type, name), entType_(type), logger_(Logger::getLogger()) {};
	int createSocket();
	bool acceptConnection(int& socket);
	int closeSocket(int& socket);
};