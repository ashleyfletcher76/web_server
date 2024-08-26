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
	ssize_t bytesRead;
	const size_t MAX_REQUEST_SIZE = 2100;
	size_t totalBytesRead = 0;

	// reads from client_socket and stores into a local buffer
	// until complete HTTP request recieved
	while (true)
	{
		bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytesRead > 0)
		{
			totalBytesRead += static_cast<size_t>(bytesRead);
			if (totalBytesRead > MAX_REQUEST_SIZE)
			{
				sendErrorResponse(client_socket, 413, "Payload too large");
				return ;
			}
			request.append(buffer, bytesRead);
			if (request.find("\r\n\r\n") != std::string::npos)
				break;
		}
		else if (bytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else if (errno == EINTR)
				continue ;
			else
			{
				logger.logMethod("ERROR", "Error reading from socket: " + std::string(strerror(errno)));
				sendErrorResponse(client_socket, 404, "Error reading from socket");
				return ;
			}
		}
		else
			return ; // connection closed
	}
	logger.logMethod("INFO", "Recieved request" + request);
	if (request.empty() || !parseHttpRequest(request, clientInfoMap[client_socket].request, client_socket))
	{
		sendErrorResponse(client_socket, 400, "Bad request");
		return;
	}
	handleRequest(client_socket);
}

// while ((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
// 	{
// 		totalBytesRead += static_cast<size_t>(bytesRead);
// 		if (totalBytesRead > MAX_REQUEST_SIZE)
// 		{
// 			sendErrorResponse(client_socket, 413, "Payload too large");
// 			return ;
// 		}
// 		request.append(buffer, bytesRead);
// 		if (request.find("\r\n\r\n") != std::string::npos)
// 			break;
// 	}
// 	if (bytesRead < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
// 	{
// 		return ;
// 	}
// 	else if (bytesRead < 0)
// 	{
// 		logger.logMethod("ERROR", "Error reading from socket: " + std::string(strerror(errno)));
// 		sendErrorResponse(client_socket, 404, "Error reading from socket");
// 		return;
// 	}
