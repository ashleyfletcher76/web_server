#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "HttpServer.hpp"

struct	routeConfig
{
	std::string path;
	std::vector<std::string> allowedMethods;
	std::string directoryListing;
	std::string handler;
};

class config
{
	private:
		//variables
		std::map<std::string, std::string> _settings;
		std::vector<routeConfig> routes;

		//methods
		bool	parseConfig(const std::string& file);

	public:
		config();
		~config();

		void	begin();
};


#endif
