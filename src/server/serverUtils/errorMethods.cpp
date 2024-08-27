#include "HttpServer.hpp"

const std::string DEFAULT_ERROR_HTML =
	"<html>"
	"<head><title>Error</title></head>"
	"<body>"
	"<h1>Error: {statusCode} - {reasonPhrase}</h1>"
	"<p>The requested operation could not be completed due to a server error.</p>"
	"</body>"
	"</html>";

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
	std::string errorFilePath = getErrorFilePath(statusCode, clientInfoMap[client_socket].server_fd);
	std::string htmlContent = readFileContent(errorFilePath);

	if (htmlContent.empty())
	{
		htmlContent = DEFAULT_ERROR_HTML;
		htmlContent = replacePlaceholder(htmlContent, "{statusCode}", std::to_string(statusCode));
		htmlContent = replacePlaceholder(htmlContent, "{reasonPhrase}", reasonPhrase);
	}

	clientInfoMap[client_socket].shouldclose = true;
	clientInfoMap[client_socket].error = true;
	std::string response = formatHttpResponse(clientInfoMap[client_socket].request.version, statusCode, reasonPhrase, htmlContent, true);
	clientResponse[client_socket] = response;
	deregisterReadEvent(client_socket);
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
