#include "HttpServer.hpp"

void	HttpServer::setupKevent(int client_socket)
{
	struct kevent change;
	EV_SET(&change, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		std::string error_msg = "Kevent registration failed for socket " + std::to_string(client_socket)
			+ ": " + std::string(strerror(errno));
		log("ERROR", error_msg, NOSTATUS);
		throw std::runtime_error(error_msg);
	}
	log("INFO", "Successfully registered kevent for socket: " + std::to_string(client_socket), NOSTATUS);
}

void HttpServer::acceptConnection()
{
	struct sockaddr_in client_address;
	socklen_t client_addrlen = sizeof(client_address);
	int client_socket = -1;
	try
	{
		client_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
		if (client_socket < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				throw std::runtime_error("Accept failed: " + std::string(strerror(errno)));
			return ; // just exit when no non-critical error
		}
		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
		log("INFO", "Accepted connection from IP: " + std::string(client_ip) + " on socket: " + std::to_string(client_socket), NOSTATUS);
		fcntl(client_socket, F_SETFL, O_NONBLOCK); // sets to non blocking mode
		setupKevent(client_socket);
		clientInfoMap[client_socket] = ClientInfo();
	}
	catch(const std::exception& e)
	{

		log("ERROR", e.what(), NOSTATUS);
		if (client_socket >= 0) // if opened succesfully but an error occured
			close(client_socket);
	}
}
