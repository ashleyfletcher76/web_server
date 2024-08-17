#include "HttpServer.hpp"

std::string HttpServer::getErrorFilePath(int statusCode, int serverFd)
{
	const serverInfo &srvInfo = servers[serverFd]->getServerInfo();

	auto it = srvInfo.errorPages.find(statusCode);
	if (it != srvInfo.errorPages.end())
	{
		return it->second;
	}
	return ("./errors/default.html");
}

void HttpServer::sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase)
{
	std::string errorFilePath = getErrorFilePath(statusCode, clientInfoMap[client_socket].server_fd);
	std::string htmlContent = readFileContent(errorFilePath);

	if (htmlContent.empty())
	{
		htmlContent = "<html><head><title>Error</title></head><body><h1>" + std::to_string(statusCode) + " " + reasonPhrase + "</h1><p>The requested method is not supported.</p></body></html>";
	}

	std::string response = formatHttpResponse(statusCode, reasonPhrase, htmlContent, clientInfoMap[client_socket].shouldclose);
	clientInfoMap[client_socket].response = response;
	writeResponse(client_socket);
}
