#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "structs.hpp"

class config
{
protected:
	size_t size;
	std::string _confile;
	std::vector<serverInfo> serverInfos;

	// Parsing methods
	bool parseConfig(const std::string &filename);
	void parseServerBlock(std::ifstream &file, serverInfo &srv);
	void parseLine(const std::string &line, serverInfo &srv);
	void parseCGIBlock(std::ifstream &file, serverInfo &srv);
	void parseRouteBlock(std::ifstream &file, serverInfo &srv);

	// Utility methods
	std::string trim(const std::string &str);
	void handleError(const std::string &message);
	void checkerrors(serverInfo &srv);

public:
	config(const std::string &confile);
	virtual ~config();

	std::string getFilename() const;
	const std::vector<serverInfo> &getServerInfos() const;

	virtual void begin();
	friend std::ostream &operator<<(std::ostream &out, const config &conf);
};

#endif