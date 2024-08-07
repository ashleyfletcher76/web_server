#include "config.hpp"

std::string config::trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");

	return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string config::getFilename() const { return (_confile); }

std::ostream &operator<<(std::ostream &out, const config &conf)
{
	out << "Configuration File: " << conf._confile << "\n\n";

	out << "Settings:\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = conf._settings.begin(); it != conf._settings.end(); ++it)
	{
		out << it->first << " = " << it->second << "\n";
	}

	out << "\nServers:\n";
	for (std::vector<server>::const_iterator srvIt = conf.servers.begin(); srvIt != conf.servers.end(); ++srvIt)
	{
		out << "Server:\n";
		for (std::vector<routeConfig>::const_iterator routeIt = srvIt->routes.begin(); routeIt != srvIt->routes.end(); ++routeIt)
		{
			out << "  Route:\n";
			out << "    Path: " << routeIt->path << "\n";
			out << "    Allowed Methods: ";
			for (std::vector<std::string>::const_iterator methodIt = routeIt->allowedMethods.begin(); methodIt != routeIt->allowedMethods.end(); ++methodIt)
			{
				out << *methodIt << " ";
			}
			out << "\n";
			out << "    Directory Listing: " << routeIt->directoryListing << "\n";
			out << "    Handler: " << routeIt->handler << "\n";
		}
		for (std::vector<cgiConfig>::const_iterator cgiIt = srvIt->cgis.begin(); cgiIt != srvIt->cgis.end(); ++cgiIt)
		{
			out << "  CGI:\n";
			out << "    Extension: " << cgiIt->extension << "\n";
			out << "    Handler: " << cgiIt->handler << "\n";
		}
	}

	return out;
}