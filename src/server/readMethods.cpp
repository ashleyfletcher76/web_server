#include "HttpServer.hpp"

std::string HttpServer::readFileContent(const std::string &filePath)
{
	// takes file path and searches for a valid file
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		logger.logMethod("ERROR", "Failed to open error file: " + filePath);
		return ("");
	}
	return (std::string((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>()));
}

void HttpServer::readRequest(int client_socket)
{
	char buffer[1024];
	std::string request;
	int bytesRead;
	const size_t MAX_REQUEST_SIZE = 2100;
	size_t totalBytesRead = 0;

	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to read from non-existent client socket : " + std::to_string(client_socket));
		return;
	}
	while ((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
	{
		totalBytesRead += static_cast<size_t>(bytesRead);
		if (totalBytesRead > MAX_REQUEST_SIZE)
		{
			sendErrorResponse(client_socket, 413, "Payload too large");
			return ;
		}
		buffer[bytesRead] = '\0';
		request.append(buffer, bytesRead);
		if (request.find("\r\n\r\n") != std::string::npos)
			break;
	}
	if (bytesRead == 0)
	{
		logger.logMethod("INFO", "Closing connection. Closed by client");
		deregisterReadEvent(client_socket);
		closeSocket(client_socket);
		return;
	}
	else if (bytesRead < 0)
	{
		logger.logMethod("ERROR", "Error reading from socket, code: " + std::to_string(bytesRead));
		sendErrorResponse(client_socket, 400, "Error reading from socket");
		return;
	}
	logger.logMethod("INFO", "Recieved request");
	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request, client_socket))
	{
		logger.logMethod("ERROR", "Error empty request! " + std::to_string(bytesRead));
		return;
	}
	handleRequest(client_socket);
}
