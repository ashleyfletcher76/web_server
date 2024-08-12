#include "config.hpp"

// Constructors
config::config(std::string confile) : _confile(confile)
{
	begin();
}

config::~config() {}

void config::begin() { parseConfig(_confile); }

bool config::parseConfig(const std::string &filename)
{
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open())
	{
		std::cerr << "Failed to open config file." << std::endl;
		return (false);
	}

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		std::cout << line << '\n';
		if (line == "server {")
		{
			parseServerBlock(file);
		}
	}
	return (true);
}

void config::parseServerBlock(std::ifstream &file)
{
	server srv;
	std::string line;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break; // End of server block

		if (line.find("route {") != std::string::npos)
		{
			parseRouteBlock(file, srv);
		}
		else if (line.find("cgi {") != std::string::npos)
		{
			parseCGIBlock(file, srv);
		}
		else
		{
			parseLine(line);
		}
	}
	servers.push_back(srv);
}

void config::parseRouteBlock(std::ifstream &file, server &srv)
{
	std::string line;
	routeConfig route;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break; // End of route block

		if (line.find("path") != std::string::npos)
		{
			route.path = line.substr(line.find(" ") + 1);
		}
		else if (line.find("allowed_methods") != std::string::npos)
		{
			std::istringstream iss(line.substr(line.find(" ") + 1));
			std::string method;
			while (iss >> method)
			{
				route.allowedMethods.push_back(method);
			}
		}
		else if (line.find("directory_listing") != std::string::npos)
		{
			route.directoryListing = line.substr(line.find(" ") + 1);
		}
		else if (line.find("handle_uploads") != std::string::npos)
		{
			route.handler = line.substr(line.find(" ") + 1);
		}
		else if (line.find("redirect") != std::string::npos)
		{
			route.handler = line.substr(line.find(" ") + 1);
		}
	}
	srv.routes.push_back(route);
}

void config::parseCGIBlock(std::ifstream &file, server &srv)
{
	std::string line;
	cgiConfig cgiHandler;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break; // End of CGI block

		if (line.find("extension") != std::string::npos)
		{
			cgiHandler.extension = line.substr(line.find(" ") + 1);
		}
		else if (line.find("handler") != std::string::npos)
		{
			cgiHandler.handler = line.substr(line.find(" ") + 1);
		}
	}
	srv.cgis.push_back(cgiHandler);
}

void config::parseLine(const std::string &line)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return; // Invalid line

	std::string key = line.substr(0, pos);
	std::string value = trim(line.substr(pos + 1));

	if (key == "listen" || key == "host" || key == "server_name" ||
		key == "document_root" || key == "default_file" ||
		key == "client_max_body_size" || key == "directory_listing")
	{
		_settings[key] = value;
	}
	else if (key == "error_page")
	{
		_settings[key] = value;
	}
}
