#include "HttpServer.hpp"

std::string getCurrentWorkingDirectory()
{
	char temp[PATH_MAX];
	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string HttpServer::readFileContent(const std::string &filePath)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		std::cerr << "Current working directory: " << getCurrentWorkingDirectory() << std::endl;
		return ("");
	}
	std::cout << "Switch between pages" << std::endl;
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void HttpServer::handleGetRequest(const std::string &path, int client_socket)
{
		log ("INFO", "Recieved GET request: /profile");
	clientInfoMap[client_socket].responseReady = true;
	if (path == "/profile")
	{
		clientInfoMap[client_socket].requestedPath = "html/profile.html";
		clientInfoMap[client_socket].statusCode = 200;
		sendResponse(client_socket);
	}
	else if (path == "/")
	{
		clientInfoMap[client_socket].requestedPath = "html/index.html";
		clientInfoMap[client_socket].statusCode = 200;
		//log ("INFO", "Recieved GET request: /profile");
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
