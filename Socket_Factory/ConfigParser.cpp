#include "ConfigParser.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <arpa/inet.h>

namespace Network {

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

bool ConfigParser::getConfigs(NetworkSettings& netSet) {
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
	
	auto familyIter = FamilyOptionMap.find(config["family"]);
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
	logger_->info_log(name_ + "Config: Server Port: " + config["server_port"].dump());
	logger_->info_log(name_ + "Config: Client Port: " + config["client_port"].dump());
	logger_->info_log(name_ + "Config: SockType: " + std::to_string(sockTypeIter->second));

	netSet.server_addr = {};
	netSet.server_addr.sin_family = familyIter->second;
	inet_pton(familyIter->second, config["ip"].dump().c_str(), &netSet.server_addr.sin_addr);
	netSet.server_port = htons(config["server_port"]);
	netSet.server_addr.sin_port = netSet.server_port;
	netSet.client_port = htons(config["client_port"]);
	netSet.sockType = sockTypeIter->second;

	return true;
};

} //Network