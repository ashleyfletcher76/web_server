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
	std::string content((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>());
	file.close();
	return (content);
}

void HttpServer::readRequest(int client_socket)
{
	char buffer[1024];
	std::string request;
	int bytesRead;
	const size_t MAX_REQUEST_SIZE = serverInfos[clientInfoMap[client_socket].server_fd].body_size;
	size_t totalBytesRead = 0;

	while ((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
	{
		totalBytesRead += bytesRead;
		if (totalBytesRead > MAX_REQUEST_SIZE)
		{
			sendErrorResponse(client_socket, 413, "Payload too large");
			return;
		}
		request.append(buffer, bytesRead);
		if (request.find("\r\n\r\n") != std::string::npos)
			break;
	}

	if (bytesRead == 0)
	{
		// Connection was closed by the client
		logger.logMethod("INFO", "Connection closed by client");
		closeSocket(client_socket);
		return;
	}
	else if (bytesRead < 0)
	{
		// Handle other errors gracefully
		logger.logMethod("ERROR", "Error reading from socket, code: " + std::to_string(bytesRead));
		sendErrorResponse(client_socket, 404, "Error reading from socket");
		return;
	}

	logger.logMethod("INFO", "Received request");

	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request, client_socket))
	{
		logger.logMethod("ERROR", "Error empty request! " + std::to_string(bytesRead));
		sendErrorResponse(client_socket, 400, "Bad request");
		return;
	}

	handleRequest(client_socket);
}