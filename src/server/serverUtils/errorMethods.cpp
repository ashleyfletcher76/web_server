#include "HttpServer.hpp"

std::string HttpServer::getErrorFilePath(int statusCode, int serverFd)
{
	const serverInfo &srvInfo = servers[serverFd]->getServerInfo();
	auto it = srvInfo.errorPages.find(statusCode);
	if (it != srvInfo.errorPages.end())
	{
		return it->second;
	}
	return ("");
}

void HttpServer::sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase)
{
	std::cout << clientInfoMap[client_socket].server_fd << "\n";
	std::string errorFilePath = getErrorFilePath(statusCode, clientInfoMap[client_socket].server_fd);
	std::cout << "Here inside send error" << std::endl;
	std::string htmlContent = readFileContent(errorFilePath);

	if (htmlContent.empty())
	{
		std::string templateContent =
			"<html>"
			"<head><title>Error</title></head>"
			"<body>"
			"<h1>Error: {statusCode} {reasonPhrase}</h1>"
			"<p>Go fuck yourself!!.</p>"
			"</body>"
			"</html>";

		htmlContent = templateContent;
		htmlContent = replacePlaceholder(htmlContent, "{statusCode}", std::to_string(statusCode));
		htmlContent = replacePlaceholder(htmlContent, "{reasonPhrase}", reasonPhrase);
	}

	std::string response = formatHttpResponse(clientInfoMap[client_socket].request.version, statusCode, reasonPhrase, htmlContent, clientInfoMap[client_socket].shouldclose);
	clientInfoMap[client_socket].response = response;
	registerWriteEvent(client_socket);
}

std::string HttpServer::replacePlaceholder(const std::string &content, const std::string &placeholder, const std::string &value)
{
	std::string result = content;
	size_t pos = 0;
	while ((pos = result.find(placeholder, pos)) != std::string::npos)
	{
		result.replace(pos, placeholder.length(), value);
		pos += value.length();
	}
	return result;
}
