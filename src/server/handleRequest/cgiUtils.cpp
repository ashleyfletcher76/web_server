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
		sendErrorResponse(client_socket, 403, "No extension on script found for: " + uri);
		return (false);
	}
	
	std::string	scriptCheck;
	pos = uri.find_last_of('/');
	if (pos != std::string::npos)
		scriptCheck = uri.substr(0, pos + 1);
	else
	{
		sendErrorResponse(client_socket, 403, "File path not suitable: " + uri);
		return (false);
	}
	for (const auto& cgi : srvInfo.cgis)
	{
		if (scriptCheck != cgi.script_alias)
		{
			sendErrorResponse(client_socket, 403, "CGI script not inside correct directory: " + uri);
			return (false);
		}
		std::string fullPath = cgi.root + uri.substr(uri.find(cgi.script_alias) + cgi.script_alias.size());
		if (!fileExist(fullPath))
		{
			sendErrorResponse(client_socket, 404, "Script not found in root: " + fullPath);
			return (false);
		}
		else if (fileExtension == cgi.extension)
		{
			if (!cgi.allowed)
			{
				logger.logMethod("ERROR", "CGI script not allowed for: " + uri);
				sendErrorResponse(client_socket, 404, "CGI script not allowed for: " + uri);
				return (false);
			}
			return (true);
		}
	}
	logger.logMethod("ERROR", "No handler found for CGI extension: " + fileExtension);
	sendErrorResponse(client_socket, 403, "No handler found for CGI extension: " + fileExtension);
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