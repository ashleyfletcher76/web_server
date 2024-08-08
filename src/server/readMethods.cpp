#include "HttpServer.hpp"

std::string HttpServer::readFileContent(const std::string& filePath, int client_socket)
{
	// takes file path and searches for a valid file
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		log("ERROR", "Failed to open error file: " + filePath, client_socket);
		return ("");
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return (content);
}

void	HttpServer::readRequest(int client_socket)
{
	char buffer[1024];
	std::string request;
	int	bytesRead;

	// reads from client_socket and stores into a local buffer
	// until complete HTTP request recieved
	while((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
	{
		request.append(buffer, bytesRead);
		if (request.find("\r\n\r\n") != std::string::npos)
			break ;
	}
	if (bytesRead < 0)
	{
		log("ERROR", "Error reading from socket: " + std::string(strerror(errno)), client_socket);
		close(client_socket);
		clientInfoMap.erase(client_socket);
		return ;
	}
	log("INFO", "Recieved request: " + request, client_socket);
	if (!parseHttpRequest(request, clientInfoMap[client_socket].request)) // stores the response from web browser and gives to method
		sendErrorResponse(client_socket, 400, "Bad request");
}

