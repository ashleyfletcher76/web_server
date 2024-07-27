#include "HttpServer.hpp"

void HttpServer::readRequest(int client_socket)
{
	// read request
	char buffer[30000] = {0};
	int valread = read(client_socket, buffer, 30000);
	if (valread <= 0)
	{
		close(client_socket);
		clientInfoMap.erase(client_socket);
		return;
	}

	// parse request to get path
	std::istringstream requestStream(buffer);
	std::string method;
	std::string path;
	requestStream >> method >> path;
	clientInfoMap[client_socket].requestedPath = path;
	clientInfoMap[client_socket].method = method;

	if (method == "GET")
		handleGetRequest(path, client_socket);
	else if (method == "POST")
		handlePostRequest(path, client_socket);
	else if (method == "DELETE")
		handleDeleteRequest(path, client_socket);
	else
		sendErrorResponse(client_socket, 405, "Method Not Allowed");

	for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); it++)
	{
		if ((*it).fd == client_socket)
		{
			(*it).events = POLLOUT;
			break ;
		}
	}
}

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
