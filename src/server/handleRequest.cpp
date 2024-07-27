#include "HttpServer.hpp"

void	HttpServer::handleGetRequest(const std::string& path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	std::cout << path << '\n';
	if (path == "/profile")
		clientInfoMap[client_socket].requestedPath = "html/profile.html";
	else if (path == "/")
		clientInfoMap[client_socket].requestedPath = "html/index.html";
	else
		clientInfoMap[client_socket].requestedPath = "html" + path;  // Default mapping
}

void	HttpServer::handlePostRequest(const std::string& path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	clientInfoMap[client_socket].requestedPath = path;
}

void	HttpServer::handleDeleteRequest(const std::string& path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	clientInfoMap[client_socket].requestedPath = path;
}
