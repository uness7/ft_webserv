#include "Config.hpp"

Config::Config(const std::string &fileName) { parseConfigFile(fileName); }

Config::~Config() {
}

void Config::parseConfigFile(const std::string &fileName) {
	std::ifstream configFile(fileName.c_str());
	std::string line;

	if (!configFile.is_open())
		throw std::runtime_error("Unable to open configuration file: " +
					 fileName);

	if (configFile.peek() == std::ifstream::traits_type::eof())
		throw std::runtime_error("Configuration file is empty: " +
					 fileName);

	bool hasServerConfig = false;

	while (std::getline(configFile, line)) {
		if (line.empty() || line[0] == '#') continue;

		if (line.find("server {") != std::string::npos) {
			ServerConfig serverConfig;
			parseServer(configFile, serverConfig);
			_serverConfigs.push_back(serverConfig);
			hasServerConfig = true;
		}
	}

	if (!hasServerConfig)
		throw std::runtime_error(
		    "Configuration file is invalid or contains only comments.");
}

long long convertToBytes(const std::string &sizeStr) {
	long long size = 0;
	char unit = 'B';
	size_t i = 0;

	while (i < sizeStr.size() && std::isdigit(sizeStr[i])) {
		size = size * 10 + (sizeStr[i] - '0');
		i++;
	}
	if (i < sizeStr.size()) unit = std::toupper(sizeStr[i]);
	switch (unit) {
		case 'K':
			size *= 1024;
			break;
		case 'M':
			size *= 1024 * 1024;
			break;
		case 'G':
			size *= 1024 * 1024 * 1024;
			break;
		default:
			break;
	}
	return size;
}

void Config::parseServer(std::ifstream &configFile,
			 ServerConfig &serverConfig) {
	std::string line;

	serverConfig.client_max_body_size = -1;
	while (std::getline(configFile, line) &&
	       line.find("}") == std::string::npos)
		parseServerLine(configFile, line, serverConfig);
}

void Config::parseServerLine(std::ifstream &configFile, const std::string &line,
			     ServerConfig &serverConfig) {
	if (line.empty() || line[0] == '#') return;

	if (line.find("listen") != std::string::npos)
		parseListen(line, serverConfig);
	else if (line.find("server_name") != std::string::npos) {
		serverConfig.server_name = extractValue(line, "server_name");
		serverConfig.fields["server_name"] = serverConfig.server_name;
	} else if (line.find("error_page") != std::string::npos) {
		serverConfig.error_page = extractValue(line, "error_page");
		serverConfig.fields["error_page"] = serverConfig.error_page;
	} else if (line.find("client_max_body_size") != std::string::npos) {
		std::string clientMaxBody =
		    extractValue(line, "client_max_body_size");
		serverConfig.client_max_body_size =
		    convertToBytes(clientMaxBody);
		serverConfig.fields["client_max_body_size"] = clientMaxBody;
	} else if (line.find("location") != std::string::npos) {
		std::string locationPath = extractLocationPath(line);
		LocationConfig locationConfig;
		locationConfig.client_max_body_size = -1;
		locationConfig.autoindex = false;
		parseLocation(configFile, locationConfig);
		serverConfig.locations[locationPath] = locationConfig;
	} else if (line.find(' ') != std::string::npos) {
		std::map<std::string, std::string> content =
		    extractUnknownValue(line);
		std::map<std::string, std::string>::iterator kv =
		    content.begin();
		serverConfig.fields[kv->first] = kv->second;
	}
}

std::map<std::string, std::string> Config::extractUnknownValue(
    const std::string &line) {
	std::map<std::string, std::string> kv;
	size_t pos = line.find(' ');
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
	key.erase(key.find_last_not_of(" \t\n\r\f\v;") + 1);
	value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
	value.erase(value.find_last_not_of(" \t\n\r\f\v;") + 1);
	kv[key] = value;
	return kv;
}

std::string Config::extractValue(const std::string &line,
				 const std::string &key) {
	size_t pos = line.find(key) + key.length();
	std::string value = line.substr(pos);
	value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
	value.erase(value.find_last_not_of(" \t\n\r\f\v;") + 1);
	return value;
}

std::string Config::extractLocationPath(const std::string &line) {
	size_t pos = line.find("location") + 8;
	size_t openBracePos = line.find("{");
	std::string locationPath = line.substr(pos, openBracePos - pos);
	locationPath.erase(0, locationPath.find_first_not_of(" \t\n\r\f\v"));
	locationPath.erase(locationPath.find_last_not_of(" \t\n\r\f\v") + 1);
	return locationPath;
}

void Config::parseListen(const std::string &line, ServerConfig &serverConfig) {
	size_t pos = line.find("listen") + 6;
	size_t colonPos = line.find(":");
	size_t semicolonPos = line.find(";");

	serverConfig.listen = line.substr(pos + 1, colonPos - pos - 1);
	std::string portStr =
	    line.substr(colonPos + 1, semicolonPos - colonPos - 1);
	serverConfig.listen.erase(
	    0, serverConfig.listen.find_first_not_of(" \t\n\r\f\v"));
	serverConfig.listen.erase(
	    serverConfig.listen.find_last_not_of(" \t\n\r\f\v") + 1);
	portStr.erase(0, portStr.find_first_not_of(" \t\n\r\f\v"));
	portStr.erase(portStr.find_last_not_of(" \t\n\r\f\v") + 1);
	std::istringstream(portStr) >> serverConfig.port;
}

void Config::parseLocation(std::ifstream &configFile,
			   LocationConfig &locationConfig) {
	std::string line;

	while (std::getline(configFile, line) &&
	       line.find("}") == std::string::npos)
		parseLocationLine(line, locationConfig);
}

void Config::parseLocationLine(const std::string &line,
			       LocationConfig &locationConfig) {
	if (line.empty() || line[0] == '#') return;

	if (line.find("root") != std::string::npos) {
		locationConfig.root = extractValue(line, "root");
		locationConfig.content["root"] = locationConfig.root;
	} else if (line.find("autoindex") != std::string::npos) {
		std::string value = extractValue(line, "autoindex");
		locationConfig.autoindex = (value == "on");
		locationConfig.content["autoindex"] = value;
	} else if (line.find("index") != std::string::npos) {
		locationConfig.index = extractValue(line, "index");
		locationConfig.content["index"] = locationConfig.index;
	} else if (line.find("limit_except") != std::string::npos) {
		std::string methods = extractValue(line, "limit_except");
		std::istringstream iss(methods);
		std::string method;
		while (iss >> method)
			locationConfig.limit_except.push_back(method);
	} else if (line.find("upload_store") != std::string::npos) {
		locationConfig.upload_store =
		    extractValue(line, "upload_store");
		locationConfig.content["upload_store"] =
		    locationConfig.upload_store;
	} else if (line.find("allowed_methods") != std::string::npos) {
		std::string methods = extractValue(line, "allowed_methods");
		std::istringstream iss(methods);
		std::string method;
		while (iss >> method)
			locationConfig.allowed_methods.push_back(method);
	} else if (line.find("cgi_script") != std::string::npos) {
		locationConfig.cgi_script = extractValue(line, "cgi_script");
		locationConfig.content["cgi_script"] =
		    locationConfig.cgi_script;
	} else if (line.find("redirect") != std::string::npos) {
		locationConfig.redirect = extractValue(line, "redirect");
		locationConfig.content["redirect"] = locationConfig.redirect;
	} else if (line.find("client_max_body_size") != std::string::npos) {
		locationConfig.content["client_max_body_size"] =
		    extractValue(line, "client_max_body_size");
		locationConfig.client_max_body_size = convertToBytes(
		    locationConfig.content["client_max_body_size"]);
	} else if (line.find(' ') != std::string::npos) {
		std::map<std::string, std::string> content =
		    extractUnknownValue(line);
		std::map<std::string, std::string>::iterator kv =
		    content.begin();
		locationConfig.content[kv->first] = kv->second;
	}
}

std::vector<ServerConfig> Config::getServerConfigs() const {
	return _serverConfigs;
}

void Config::printConfigs(const std::vector<ServerConfig> &serverConfigs) {
	for (std::vector<ServerConfig>::const_iterator it =
		 serverConfigs.begin();
	     it != serverConfigs.end(); ++it)
		printServerConfig(*it);
}

void Config::printServerConfig(const ServerConfig &serverConfig) {
	std::cout << "Server listening on " << serverConfig.listen
		  << " Port: " << serverConfig.port << std::endl;
	std::map<std::string, std::string>::const_iterator f;
	for (f = serverConfig.fields.begin(); f != serverConfig.fields.end();
	     f++)
		std::cout << "\t" << f->first << " " << f->second << std::endl;
	std::cout << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator loc_it =
		 serverConfig.locations.begin();
	     loc_it != serverConfig.locations.end(); ++loc_it)
		printLocationConfig(loc_it->first, loc_it->second);
}

void Config::printLocationConfig(const std::string &locationPath,
				 const LocationConfig &locationConfig) {
	std::cout << "\tLocation: " << locationPath << std::endl;

	std::map<std::string, std::string>::const_iterator f;
	for (f = locationConfig.content.begin();
	     f != locationConfig.content.end(); f++)
		std::cout << "\t\t" << f->first << " " << f->second << std::endl;

	std::cout << std::endl;
}

short ServerConfig::getLocationByPathRequested(std::string path,
					       LocationConfig &target) {
	std::map<std::string, LocationConfig> lc = locations;
	std::map<std::string, LocationConfig>::const_iterator it;
	int path_max = -1;

	for (it = lc.begin(); it != lc.end(); it++) {
		int key_size = it->first.size();
		int found = it->first.compare(0, key_size, path, 0, key_size);
		if (found == 0 && path_max < key_size) {
			target = it->second;
			path_max = key_size;
		}
	}
	return path_max;
}
