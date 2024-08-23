#include "HttpServer.hpp"

void HttpServer::configureSocketNonBlocking(int client_socket)
{
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::string errMsg = "Failed to set non-blocking mode for socket: " + std::to_string(client_socket);
		logger.logMethod("ERROR", errMsg);
		throw std::runtime_error(errMsg);
	}
	logger.logMethod("Info", "Socket configured to non-blocking mode: " + std::to_string(client_socket));
}

void HttpServer::acceptConnection(int serverSocket)
{
	struct sockaddr_in client_address;
	socklen_t client_addrlen = sizeof(client_address);
	int client_socket = accept(serverSocket, (struct sockaddr *)&client_address, &client_addrlen);
	if (client_socket < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			logger.logMethod("ERROR", "Accept failed: " + std::string(strerror(errno)));
		}
		return;
	}
	openSockets.insert(client_socket);

	char client_ip[INET_ADDRSTRLEN];
	if (!inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN))
	{
		logger.logMethod("ERROR", "Connection failed: " + std::string(strerror(errno)));
		return ;
	}
	logger.logMethod("INFO", "Accepted connection from IP: " + std::string(client_ip) + " on socket: " + std::to_string(client_socket));

	configureSocketNonBlocking(client_socket);
	registerReadEvent(client_socket);
	clientInfoMap.emplace(client_socket, ClientInfo(serverSocket));
}
