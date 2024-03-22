#pragma once

#include <netinet/in.h>


// Easy to switch tested type
#define USED_TYPE uint32_t

namespace Network {

enum class EntityType {
	Server,
	Client
};

class NetworkSettings {
public:
	int sockType;
	struct sockaddr_in server_addr;
	in_port_t client_port;
	in_port_t server_port;
};

} // Network