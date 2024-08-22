#include "HttpServer.hpp"

std::string HttpServer::readFileContent(const std::string& filePath)
{
	// takes file path and searches for a valid file
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		logger.logMethod("ERROR", "Failed to open error file: " + filePath);
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
	const size_t MAX_REQUEST_SIZE = 2100;
	size_t totalBytesRead = 0;

	// reads from client_socket and stores into a local buffer
	// until complete HTTP request recieved
	while((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
	{
		totalBytesRead += bytesRead;
		if (totalBytesRead > MAX_REQUEST_SIZE)
		{
			logger.logMethod("ERROR", "Request too large");
			sendErrorResponse(client_socket, 413, "Payload too large");
			closeSocket(client_socket);
			return ;
		}
		request.append(buffer, bytesRead);
		if (request.find("\r\n\r\n") != std::string::npos)
			break ;
	}
	if (bytesRead < 0)
	{
		logger.logMethod("ERROR", "Error reading from socket: " + std::string(strerror(errno)));
		std::cout << "Here inside bytes read error" << std::endl;
		sendErrorResponse(client_socket, 404, "Error reading from socket");
		closeSocket(client_socket);
		return ;
	}
	logger.logMethod("INFO", "Recieved request: " + request);
	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request, client_socket)) // stores the response from web browser and gives to method
	{
		sendErrorResponse(client_socket, 400, "Bad request");
		closeSocket(client_socket);
	}
}

