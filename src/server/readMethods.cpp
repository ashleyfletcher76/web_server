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

void HttpServer::readRequest(int client_socket)
{
	char buffer[1024];
	int bytesRead;

	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to read from non-existent client socket: " + std::to_string(client_socket));
		return;
	}

	bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);

	if (bytesRead > 0)
	{
		clientInfoMap[client_socket].requestBuffer.append(buffer, bytesRead);

		if (clientInfoMap[client_socket].requestBuffer.find("\r\n\r\n") != std::string::npos)
		{
			if (clientInfoMap[client_socket].requestBuffer.size() > static_cast<size_t>(getMaxClientBodySize(client_socket)))
			{
				sendErrorResponse(client_socket, 413, "Payload too large");
				return;
			}

			if (handleHeadersAndCheckForBody(client_socket))
			{
				return;
			}
			else
			{
				if (!parseHttpRequest(clientInfoMap[client_socket].requestBuffer, clientInfoMap[client_socket].request, client_socket))
				{
					logger.logMethod("ERROR", "Error parsing request");
					sendErrorResponse(client_socket, 400, "Bad Request");
					return ;
				}
				logger.logMethod("INFO", "Received request: " + clientInfoMap[client_socket].request.method);
				handleRequest(client_socket);
				clientInfoMap[client_socket].requestBuffer.clear();
				return;
			}
		}
	}
	else if (bytesRead == 0)
	{
		logger.logMethod("WARNING", "Connection closed by client");
		deregisterReadEvent(client_socket);
		closeSocket(client_socket);
	}
	else if (bytesRead < 0)
	{
		logger.logMethod("WARNING", "Connection temporarly not availabe with the socket");
	}
}

bool HttpServer::handleHeadersAndCheckForBody(int client_socket)
{
	auto &clientInfo = clientInfoMap[client_socket];
	std::string::size_type contentLengthPos = clientInfo.requestBuffer.find("Content-Length: ");

	if (contentLengthPos != std::string::npos)
	{
		return readFullRequestBody(client_socket, contentLengthPos);
	}

	return false;
}

bool HttpServer::readFullRequestBody(int client_socket, std::string::size_type contentLengthPos)
{
	auto &clientInfo = clientInfoMap[client_socket];
	size_t contentLength = std::stoi(clientInfo.requestBuffer.substr(contentLengthPos + 16));

	size_t bodyStart = clientInfo.requestBuffer.find("\r\n\r\n") + 4;
	size_t currentBodyLength = clientInfo.requestBuffer.size() - bodyStart;

	if (currentBodyLength < contentLength)
	{
		return true;
	}

	if (currentBodyLength > static_cast<size_t>(getMaxClientBodySize(client_socket)))
	{
		sendErrorResponse(client_socket, 413, "Payload too large.");
		return false;
	}

	return false;
}