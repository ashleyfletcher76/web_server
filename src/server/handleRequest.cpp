#include "HttpServer.hpp"

// std::string getCurrentWorkingDirectory()
// {
// 	char temp[PATH_MAX];
// 	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
// }

std::string HttpServer::readFileContent(const std::string &filePath, int client_socket)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		clientInfoMap[client_socket].statusCode = 404;
		log ("ERROR", "HTTP/1.1 " + std::to_string(clientInfoMap[client_socket].statusCode) + " FAILED - " " Recieved GET request: " + clientInfoMap[client_socket].requestedPath
			+ "\nContent-Type: text/html\nContent-Length: ", client_socket);
		return ("");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void HttpServer::handleGetRequest(const std::string &path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	if (path == "/profile.html")
	{
		clientInfoMap[client_socket].requestedPath = "html/profile.html";
		clientInfoMap[client_socket].statusCode = 200;
		sendResponse(client_socket);
	}
	else if (path == "/" || path == "/index.html")
	{
		clientInfoMap[client_socket].requestedPath = "html/index.html";
		clientInfoMap[client_socket].statusCode = 200;
		sendResponse(client_socket);
	}
	else
	{
		clientInfoMap[client_socket].requestedPath = "html" + path;
		clientInfoMap[client_socket].statusCode = 200;
		sendResponse(client_socket);
	}
}

void HttpServer::handlePostRequest(const std::string &path, int client_socket, char buffer[30000])
{
	ClientInfo &clientInfo = clientInfoMap[client_socket];
	std::string headers(buffer);
	size_t contentLengthPos = headers.find("Content-Length: ");
	if (contentLengthPos != std::string::npos)
	{
		size_t start = contentLengthPos + 16;
		size_t end = headers.find("\r\n", start);
		int content_length = std::stoi(headers.substr(start, end - start));
		size_t bodyStart = headers.find("\r\n\r\n");
		if (bodyStart != std::string::npos)
		{
			std::string postData = headers.substr(bodyStart, content_length);
			clientInfo.postData = postData;
			clientInfo.requestedPath = path;
			clientInfo.responseReady = true;
		}
		else
		{
			std::cerr << "Failed to find the start of the post data" << std::endl;
		}
	}
	else
	{
		std::cerr << "Content-Length not found" << std::endl;
	}
}

void HttpServer::handleDeleteRequest(const std::string &path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	clientInfoMap[client_socket].requestedPath = path;
}
