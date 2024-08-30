#include "config.hpp"

config::config(const std::string &confile) : size(0), _confile(confile)
{
	begin();
}

config::~config() {}

void config::begin()
{
	if (!parseConfig(_confile))
	{
		handleError("Failed to parse configuration file.");
	}
}

bool config::parseConfig(const std::string &filename)
{
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open())
	{
		handleError("Failed to open config file: " + filename);
		return false;
	}

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (line == "server {")
		{
			serverInfo srv;
			try
			{
				parseServerBlock(file, srv);
				serverInfos.push_back(srv);
				++size;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
	if (serverInfos.empty())
	{
		return (false);
	}
	return true;
}

void config::parseServerBlock(std::ifstream &file, serverInfo &srv)
{
	std::string line;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "}")
			break;
		if (line.find("error_page") != std::string::npos)
		{
			std::istringstream iss(line.substr(line.find(" ") + 1));
			int statusCode;
			std::string errorPagePath;
			iss >> statusCode >> errorPagePath;
			srv.errorPages[statusCode] = errorPagePath;
		}
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
	checkerrors(srv);
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
			route.directoryListing = (line.find("on") || line.find("1") || line.find("true"));
		}
		else if (line.find("handle_uploads") != std::string::npos)
		{
			route.handler = line.substr(line.find(" ") + 1);
		}
		else if (line.find("index") != std::string::npos)
		{
			route.index = line.substr(line.find(" ") + 1);
		}
		else if (line.find("defaultFile") != std::string::npos)
		{
			route.defaultFile = line.substr(line.find(" ") + 1);
		}
		else if (line.find("rootDirectory") != std::string::npos)
		{
			route.rootDirectory = line.substr(line.find(" ") + 1);
		}
		else if (line.find("redirect") != std::string::npos)
		{
			route.redirect = line.substr(line.find(" ") + 1);
		}
	}

	if (!route.path.empty())
	{
		srv.routes[route.path] = route;
	}
	else
	{
		handleError("Route block does not specify a path.");
	}
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
		else if (line.find("script_alias") != std::string::npos)
		{
			cgiHandler.script_alias = line.substr(line.find(" ") + 1);
		}
		else if (line.find("root") != std::string::npos)
		{
			cgiHandler.root = line.substr(line.find(" ") + 1);
		}
		else if (line.find("allowed") != std::string::npos)
		{
			if (line.find("on") != std::string::npos)
				cgiHandler.allowed = true;
			else
				cgiHandler.allowed = false;
		}
	}
	srv.cgis.push_back(cgiHandler);
}

void config::parseLine(const std::string &line, serverInfo &srv)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return;

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
		int size = std::atoi(value.c_str());
		if (size > 0)
		{
			srv.body_size = size;
		}
		else
		{
			handleError("Invalid client_max_body_size: " + value);
		}
	}
	else if (key == "directory_listing")
	{
		srv.directory_listing = (value == "on" || value == "true" || value == "1");
	}
}

void config::handleError(const std::string &message)
{
	throw std::runtime_error(message);
}

const std::vector<serverInfo> &config::getServerInfos() const
{
	return serverInfos;
}
