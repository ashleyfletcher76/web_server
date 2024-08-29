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
		size_t contentLength = std::stoi(request.substr(lengthStart, lengthEnd - lengthStart));
		size_t bodyEnd = request.find("\r\n\r\n") + 4;
		size_t requiredBytes = bodyEnd + contentLength;

		if (totalBytesRead < requiredBytes)
		{
			while (totalBytesRead < requiredBytes)
			{
				bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
				if (bytesRead > 0)
				{
					totalBytesRead += static_cast<size_t>(bytesRead);
					request.append(buffer, bytesRead);
				}
				else if (bytesRead < 0)
				{
					logger.logMethod("ERROR", "Error reading from socket, code: " + std::to_string(bytesRead));
					sendErrorResponse(client_socket, 400, "Error reading from socket");
					return false;
				}
				else if (bytesRead == 0)
				{
					logger.logMethod("INFO", "Connection closed by client");
					break;
				}
			}
			if (totalBytesRead > static_cast<size_t>(getMaxClientBodySize(client_socket)))
			{
				sendErrorResponse(client_socket, 413, "Payload too large.");
				return false;
			}
		}
		else
		{
			if (totalBytesRead < requiredBytes)
			{
				sendErrorResponse(client_socket, 400, "Bad request");
				return false;
			}
		}
	}
	else
	{
		logger.logMethod("ERROR", "Content-Length header is missing or malformed");
		sendErrorResponse(client_socket, 400, "Bad request");
		return false;
	}

	return true;
}

void HttpServer::readRequest(int client_socket)
{
	char buffer[4096];
	std::string request;
	int bytesRead;
	const size_t MAX_REQUEST_SIZE = 4096 * 2; // Adjust as needed
	size_t totalBytesRead = 0;

	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to read from non-existent client socket: " + std::to_string(client_socket));
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
		logger.logMethod("INFO", "Connection closed by client");
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
		if (!readFullRequestBody(client_socket, request, contentLengthPos, totalBytesRead, bytesRead))
		{
			return;
		}
	}

	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request, client_socket))
	{
		logger.logMethod("ERROR", "Error parsing request");
		return;
	}

	logger.logMethod("INFO", "Received request: " + clientInfoMap[client_socket].request.method);
	handleRequest(client_socket);
}