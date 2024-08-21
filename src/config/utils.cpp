#include "config.hpp"

std::string config::trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");

	return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

void config::checkerrors(serverInfo &srv)
{
	if (srv.listen < 1 || srv.listen > 65535)
	{
		handleError("Invalid port number: " + std::to_string(srv.listen));
	}
	if (srv.body_size < 1)
	{
		handleError("Invalid body size: " + std::to_string(srv.body_size));
	}

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
		out << "    client_max_body_size : " << srvIt->body_size << '\n';
		out << "    directory_listing    : " << srvIt->directory_listing << '\n';
		for (const auto &pair : srvIt->errorPages)
		{
			out << "    errorpages           : Status Code: " << pair.first << " -> Error Page: " << pair.second << std::endl;
		}
		for (auto routeIt = srvIt->routes.begin(); routeIt != srvIt->routes.end(); ++routeIt)
		{
			out << "  Route:\n";
			out << "    Path: " << routeIt->first << "\n"; // routeIt->first contains the path (key)

			const routeConfig &route = routeIt->second; // routeIt->second contains the routeConfig (value)

			out << "    Allowed Methods: ";
			for (std::vector<std::string>::const_iterator methodIt = route.allowedMethods.begin(); methodIt != route.allowedMethods.end(); ++methodIt)
			{
				out << *methodIt << " ";
			}
			out << "\n";

			out << "    Directory Listing: " << (route.directoryListing ? "enabled" : "disabled") << "\n";
			out << "    Handler: " << route.handler << "\n";

			// Add the redirect field if applicable
			if (!route.redirect.empty())
			{
				out << "    Redirect: " << route.redirect << "\n";
			}
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