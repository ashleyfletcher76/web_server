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
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

std::string HttpServer::readPostData(int content_length)
{
	std::string postData;
	postData.resize(content_length);
	read(STDIN_FILENO, &postData[0], content_length);
	return postData;
}

void HttpServer::handleGetRequest(const std::string &path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	if (path == "/profile")
		clientInfoMap[client_socket].requestedPath = "html/profile.html";
	else if (path == "/")
		clientInfoMap[client_socket].requestedPath = "html/index.html";
	else
		clientInfoMap[client_socket].requestedPath = "html" + path; // Default mapping
}

void HttpServer::handlePostRequest(const std::string &path, int client_socket, char buffer[30000])
{
	ClientInfo &clientInfo = clientInfoMap[client_socket];
	size_t contentLengthPos = std::string(buffer).find("Content-Length: ");
	if (contentLengthPos != std::string::npos)
	{
		size_t start = contentLengthPos + 16;
		size_t end = std::string(buffer).find("\r\n", start);
		int content_length = std::stoi(std::string(buffer).substr(start, end - start));
		clientInfo.postData = readPostData(content_length);
		clientInfo.requestedPath = path;
		clientInfo.responseReady = true;
	}
}

void HttpServer::handleDeleteRequest(const std::string &path, int client_socket)
{
	clientInfoMap[client_socket].responseReady = true;
	clientInfoMap[client_socket].requestedPath = path;
}
