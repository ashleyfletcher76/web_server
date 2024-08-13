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

	for (std::vector<serverInfo>::const_iterator srvIt = conf.serverInfos.begin(); srvIt != conf.serverInfos.end(); ++srvIt)
	{
		out << "ServerInfo:\n";
		out << "  Settings:\n";
		out << "    listen               : " << srvIt->listen << '\n';
		out << "    host                 : " << srvIt->host << '\n';
		out << "    server_name          : " << srvIt->server_name << '\n';
		out << "    document_root        : " << srvIt->document_root << '\n';
		out << "    default_file         : " << srvIt->default_file << '\n';
		out << "    client_max_body_size : " << srvIt->client_max_body_size << '\n';
		out << "    directory_listing    : " << srvIt->directory_listing << '\n';
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