#include "ConfigParser.h"
#include <fstream>
#include <cstring> // used for memset
#include <nlohmann/json.hpp>
#include <arpa/inet.h>

// !!! remove
#include <iostream>

using json = nlohmann::json;
															
const std::map<std::string, int> ConfigParser::FamilyOptionMap = { {"AF_LOCAL", AF_LOCAL},
																   {"AF_INET", AF_INET},
																   {"AF_INET6", AF_INET6},
																   {"AF_UNIX", AF_UNIX},
																   {"AF_UNSPEC", AF_UNSPEC} };
const std::map<std::string, int> ConfigParser::SockTypeMap = { {"SOCK_DGRAM", SOCK_DGRAM},
															   {"SOCK_RAW", SOCK_RAW},
															   {"SOCK_SEQPACKET", SOCK_SEQPACKET},
															   {"SOCK_STREAM", SOCK_STREAM} };

bool ConfigParser::getConfigs(struct sockaddr_in& server_addr, int& socketType) {
	if (!fsys::exists(CONFIG_PATH_)) {
		logger_->error_log(name_ + "Config: file doesn't exist");
		return false;
	}


	std::ifstream stream(CONFIG_PATH_, std::ifstream::in);
	if (!stream.good()) {
		logger_->error_log(name_ + "config stream is bad");
		return false;
	}

	auto config = json::parse(stream);

	if (entType_ == EntityType::Server) {
		config = config["servers"][name_];
	}
	else {
		config = config["clients"][name_];
	}

	logger_->info_log(name_ + "Config: " + config.dump());
	
	auto familyIter = FamilyOptionMap.find(config["family"]); // !!! create enum for config options
	auto sockTypeIter = SockTypeMap.find(config["sock_type"]);
	if(familyIter == FamilyOptionMap.end() || sockTypeIter == SockTypeMap.end()) {
		logger_->error_log(name_ + "Config: can't find socket family or type");
		return false;
	}
	
	if(config["ip"].dump().empty() || config["port"].dump().empty()) {
		logger_->error_log(name_ + "Config: can't find ip or port");
		return false;
	}
	logger_->info_log(name_ + "Config: Family: " + std::to_string(familyIter->second));
	logger_->info_log(name_ + "Config: IP: " + config["ip"].dump());
	logger_->info_log(name_ + "Config: Port: " + config["port"].dump());
	logger_->info_log(name_ + "Config: SockType: " + std::to_string(sockTypeIter->second));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = familyIter->second;
	server_addr.sin_addr.s_addr = inet_addr(config["ip"].dump().c_str()); // inet_addr(argv[1]);
	server_addr.sin_port = htons(config["port"]);
	socketType = sockTypeIter->second;

	return true;
};