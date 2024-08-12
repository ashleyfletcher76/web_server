#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

struct routeConfig
{
	std::string path;
	std::vector<std::string> allowedMethods;
	std::string directoryListing;
	std::string handler;
};

struct cgiConfig
{
	std::string extension;
	std::string handler;
};

struct serverInfo
{
	int				listen;
	std::string		host;
	std::string		server_name;
	std::string		document_root;
	std::string		default_file;
	std::string		client_max_body_size;
	std::string		directory_listing;
	std::vector<routeConfig> routes;
	std::vector<cgiConfig> cgis;
};

class config
{
protected:
	// variables
	size_t					size;
	std::string				_confile;
	std::vector<serverInfo>	serverInfos;

	// methods
	bool parseConfig(const std::string &filename);
	void parseServerBlock(std::ifstream &file, serverInfo &srv);
	void parseLine(const std::string &line, serverInfo &srv);
	void parseCGIBlock(std::ifstream &file, serverInfo &srv);
	void parseRouteBlock(std::ifstream &file, serverInfo &srv);

	// utils
	std::string trim(const std::string &str);

public:
	config(std::string confile);
	virtual ~config();

	std::string getFilename() const;

	virtual void begin();
	friend std::ostream &operator<<(std::ostream &out, const config &conf);
};

#endif
