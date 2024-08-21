#include "HttpServer.hpp"

// setup to read kevent events for sockets
void HttpServer::setupKevent(int client_socket, int timeoutSeconds)
{
	struct kevent changes[2];
	int changeCount = 1;

	// always reset the existing kevents for the socket first
	EV_SET(&changes[0], static_cast<uintptr_t>(client_socket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	EV_SET(&changes[1], static_cast<uintptr_t>(client_socket), EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
	kevent(kq, &changes[1], 1, NULL, 0, NULL);  // attempt to remove existing timer if it exists

	// set up a timer event if keep-alive is enabled
	if (timeoutSeconds > 0)
	{
		EV_SET(&changes[1], static_cast<uintptr_t>(client_socket), EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, timeoutSeconds, NULL);
		changeCount = 2;
	}

	if (kevent(kq, changes, changeCount, NULL, 0, NULL) == -1)
	{
		std::string error_msg = "Kevent registration failed for socket " + std::to_string(client_socket) + ": " + strerror(errno);
		logger.logMethod("ERROR", error_msg);
		throw std::runtime_error(error_msg);
	}
	logger.logMethod("INFO", "Successfully updated kevent for socket: " + std::to_string(client_socket));
}



// configure a given socket for non-blocking mode
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
	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
	logger.logMethod("INFO", "Accepted connection from IP: " + std::string(client_ip) + " on socket: " + std::to_string(client_socket));

	configureSocketNonBlocking(client_socket);
	setupKevent(client_socket, 10);
	clientInfoMap.emplace(client_socket, ClientInfo(serverSocket));
}
