#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

struct	routeConfig
{
	std::string path;
	std::vector<std::string> allowedMethods;
	std::string directoryListing;
	std::string handler;
};

struct	cgiConfig
{
	std::string extension;
	std::string handler;
};

struct	server
{
	std::vector<routeConfig>	routes;
	std::vector<cgiConfig>		cgis;
};

class config
{
	protected:
		//variables
		std::string	_confile;
		std::map<std::string, std::string> _settings;
		std::vector<server> servers;

		//methods
		bool	parseConfig(const std::string &filename);
		void	parseServerBlock(std::ifstream& file);
		void	parseLine(const std::string &line);
		void	parseCGIBlock(std::ifstream &file, server &srv);
		void	parseRouteBlock(std::ifstream &file, server &srv);

		//utils
		std::string trim(const std::string& str);

	public:
		config(std::string confile);
		~config();
		
		std::string getFilename() const;

		void	begin();
		friend std::ostream& operator<<(std::ostream& out, const config& conf);

};


#endif
