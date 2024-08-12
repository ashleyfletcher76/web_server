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
	std::string content((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	file.close();
	return (content);
}

void	HttpServer::readRequest(int client_socket)
{
	char buffer[1024];
	std::string request;
	int	bytesRead;
	const size_t MAX_REQUEST_SIZE = 8192; // max size (8kb)
	size_t totalBytesRead = 0;

	// reads from client_socket and stores into a local buffer
	// until complete HTTP request recieved
	while((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
	{
		totalBytesRead += bytesRead;
		if (totalBytesRead > MAX_REQUEST_SIZE)
		{
			log("ERROR", "Request too large", NOSTATUS);
			sendErrorResponse(client_socket, 413, "Payload too large");
			closeSocket(client_socket);
			clientInfoMap.erase(client_socket);
			return ;
		}
		request.append(buffer, bytesRead);
		if (request.find("\r\n\r\n") != std::string::npos)
			break ;
	}
	if (bytesRead < 0)
	{
		log("ERROR", "Error reading from socket: " + std::string(strerror(errno)), client_socket);
		closeSocket(client_socket);
		//close(client_socket);
		clientInfoMap.erase(client_socket);
		return ;
	}
	log("INFO", "Recieved request: " + request, client_socket);
	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request)) // stores the response from web browser and gives to method
	{
		closeSocket(client_socket);
		clientInfoMap.erase(client_socket);
		sendErrorResponse(client_socket, 400, "Bad request");
	}
}

