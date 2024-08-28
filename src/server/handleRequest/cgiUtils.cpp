#include "HttpServer.hpp"

bool	fileExist(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool	HttpServer::checkIfCgiAllowed(const std::string& uri, int client_socket)
{
	auto serverIt = servers.find(clientInfoMap[client_socket].server_fd);
	const serverInfo &srvInfo = serverIt->second->getServerInfo();

	std::string	fileExtension;
	std::size_t	pos = uri.find_last_of('.');

	if (pos != std::string::npos)
		fileExtension = uri.substr(pos);
	else
	{
		clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 403, "Forbidden", "No extension on script found for: " + uri, true);
		return (false);
	}
	
	std::string	scriptCheck;
	pos = uri.find_last_of('/');
	if (pos != std::string::npos)
		scriptCheck = uri.substr(0, pos + 1);
	else
	{
		clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 403, "Forbidden", "File path not suitable: " + uri, true);
		return (false);
	}
	for (const auto& cgi : srvInfo.cgis)
	{
		if (scriptCheck != cgi.script_alias)
		{
			clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 403, "Forbidden", "CGI script not inside correct directory: " 
					+ uri, true);
			return (false);
		}
		std::string fullPath = cgi.root + uri.substr(uri.find(cgi.script_alias) + cgi.script_alias.size());
		std::cout << fullPath << std::endl;
		if (!fileExist(fullPath))
		{
			clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 404, "Not Found", "Script not found in root: " 
				+ fullPath, true);
			return (false);
		}
		else if (fileExtension == cgi.extension)
		{
			if (!cgi.allowed)
			{
				logger.logMethod("ERROR", "CGI script not allowed for: " + uri);
				clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 403, "Forbidden", "CGI script not allowed for: " 
					+ uri, true);
				return (false);
			}
			return (true);
		}
	}
	logger.logMethod("ERROR", "No handler found for CGI extension: " + fileExtension);
	clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 403, "Forbidden", "No handler found for CGI extension: " 
		+ fileExtension, true);
	return (false);
}

std::string	HttpServer::findHandler(const std::string& uri, int client_socket)
{
	auto serverIt = servers.find(clientInfoMap[client_socket].server_fd);
	const serverInfo &srvInfo = serverIt->second->getServerInfo();

	std::string	fileExtension;
	std::size_t	pos = uri.find_last_of('.');

	if (pos != std::string::npos)
		fileExtension = uri.substr(pos);
	else
		return ("");
	
	for (const auto& cgi : srvInfo.cgis)
	{
		if (fileExtension == cgi.extension)
		{
			return (cgi.handler);
		}
	}
	return ("");
}

std::string	HttpServer::parseCgiOutput(std::string cgiOutput)
{
	// Parsing CGI output
	std::string body;
	std::string header;
	bool headerFinished = false;
	std::istringstream iss(cgiOutput);
	std::string line;
	while (std::getline(iss, line))
	{
		if (!headerFinished)
		{
			if (line.empty() || line == "\r")
				headerFinished = true;
			else
				header += line + "\n";
		}
		else
			body += line + "\n";
	}
	return (body);
}