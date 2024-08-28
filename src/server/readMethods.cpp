#include "HttpServer.hpp"

std::string HttpServer::readFileContent(const std::string &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		logger.logMethod("ERROR", "Failed to open error file: " + filePath);
		return ("");
	}
	return (std::string((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>()));
}

bool HttpServer::readFullRequestBody(int client_socket, std::string &request, std::string::size_type contentLengthPos, size_t totalBytesRead, int bytesRead)
{
	char buffer[4096];
	std::string::size_type lengthStart = contentLengthPos + 16;
	std::string::size_type lengthEnd = request.find("\r\n", lengthStart);
	if (lengthEnd != std::string::npos)
	{
		int contentLength = std::stoi(request.substr(lengthStart, lengthEnd - lengthStart));
		if (totalBytesRead < static_cast<size_t>(contentLength) + request.find("\r\n\r\n"))
		{
			while (totalBytesRead < static_cast<size_t>(contentLength) + request.find("\r\n\r\n"))
			{
				bytesRead = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
				if (bytesRead > 0)
				{
					totalBytesRead += static_cast<size_t>(bytesRead);
					buffer[bytesRead] = '\0';
					request.append(buffer, bytesRead);
				}
				else if (bytesRead < 0)
				{
					logger.logMethod("ERROR", "Error reading from socket, code: " + std::to_string(bytesRead));
					sendErrorResponse(client_socket, 400, "Error reading from socket");
					return false;
				}
			}
		}
	}
	return (true);
}

void HttpServer::readRequest(int client_socket)
{
	char buffer[4096];
	std::string request;
	int	bytesRead;
	const size_t MAX_REQUEST_SIZE = 4096 * 2;
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
			return;
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

	std::string::size_type contentLengthPos = request.find("Content-Length: ");
	if (contentLengthPos != std::string::npos)
	{
		if (!readFullRequestBody(client_socket, request, contentLengthPos, totalBytesRead,  bytesRead))
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
