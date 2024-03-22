#pragma once

#include <string>
#include <filesystem>
#include <netinet/in.h>
#include <map>

#include "SharedTypes.h"
#include "Logger.h"


namespace Network {

namespace fsys = std::filesystem;

class ConfigParser {
private:
	const fsys::path CONFIG_PATH_ = "Configs/socket_config.json";
	EntityType entType_;
	std::string name_;
	std::shared_ptr<Logger> logger_;

public:
	static const std::map<std::string, int> FamilyOptionMap;
	static const std::map<std::string, int> SockTypeMap;

	ConfigParser(EntityType& type, std::string& name) : entType_(type), name_(name), logger_(Logger::getLogger()) {};
	bool getConfigs(NetworkSettings& netSet);
};

} //Network