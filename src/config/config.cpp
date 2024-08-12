#include "config.hpp"

// Constructors
config::config(std::string confile) : size(0), _confile(confile)
{
	begin();
	std::cout << *this << '\n';
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
		if (line == "server {")
		{
			serverInfo srv;
			parseServerBlock(file, srv);
			serverInfos.push_back(srv);
		}
	}
	return (true);
}

void config::parseServerBlock(std::ifstream &file, serverInfo &srv)
{
	std::string line;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break;

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
			parseLine(line, srv);
		}
	}
}

void config::parseRouteBlock(std::ifstream &file, serverInfo &srv)
{
	std::string line;
	routeConfig route;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break;

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

void config::parseCGIBlock(std::ifstream &file, serverInfo &srv)
{
	std::string line;
	cgiConfig cgiHandler;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break;
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

void config::parseLine(const std::string &line, serverInfo &srv)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return ;

	std::string key = line.substr(0, pos);
	std::string value = trim(line.substr(pos + 1));

	if (key == "listen")
	{
		srv.listen = std::atoi(value.c_str());
	}
	else if (key == "host")
	{
		srv.host = value;
	}
	else if (key == "server_name")
	{
		srv.server_name = value;
	}
	else if (key == "document_root")
	{
		srv.document_root = value;
	}
	else if (key == "default_file")
	{
		srv.default_file = value;
	}
	else if (key == "client_max_body_size")
	{
		srv.client_max_body_size = value;
	}
	else if (key == "directory_listing")
	{
		srv.directory_listing = value;
	}
}
