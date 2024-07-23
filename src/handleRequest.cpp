#include "HttpServer.hpp"

void	HttpServer::handleGetRequest(const std::string& path)
{
	if (path == "/profile")
		requestedPath = "html/profile.html";  // Correct file path
	else if (path == "/")
		requestedPath = "html/index.html";
	else
		requestedPath = "html" + path;  // Default mapping
	sendResponse();
}

void	HttpServer::handlePostRequest(const std::string& path)
{
	(void)path;
	sendResponse();
}

void	HttpServer::handleDeleteRequest(const std::string& path)
{
	(void)path;
	sendResponse();
}
