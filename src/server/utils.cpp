#include "HttpServer.hpp"

std::string HttpServer::formatHttpResponse(int status_code, const std::string &reasonPhrase,
										   const std::string &body)
{
	std::ostringstream response;

	// constructs proper format for HTTP response
	response << "HTTP/1.1 " << status_code << " " << reasonPhrase << "\r\n";
	response << "Content-Length: " << body.size() << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << body;
	return (response.str());
}

void HttpServer::closeSocket(int client_socket)
{
	struct kevent change;
	EV_SET(&change, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
		logger.logMethod("ERROR", "Failed to remove read event from kqueue for FD: " + std::to_string(client_socket), NOSTATUS);
	EV_SET(&change, client_socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
		logger.logMethod("ERROR", "Failed to remove write event from kqueue for FD: " + std::to_string(client_socket), NOSTATUS);
	close(client_socket);
	openSockets.erase(client_socket);
	logger.logMethod("INFO", "Closed client socket FD: " + std::to_string(client_socket), NOSTATUS);
}

std::string HttpServer::getFilePath(const std::string &uri)
{
	return ("html" + uri);
}

bool is_socket_bound(int socket_fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	if (getsockname(socket_fd, (struct sockaddr *)&addr, &addr_len) == -1)
	{
		std::cerr << "Error checking if socket is bound: " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

bool is_socket_listening(int socket_fd)
{
	int listening = 0;
	socklen_t opt_len = sizeof(listening);

	if (getsockopt(socket_fd, SOL_SOCKET, SO_ACCEPTCONN, &listening, &opt_len) == -1)
	{
		std::cerr << "Error checking if socket is listening: " << strerror(errno) << std::endl;

		if (errno == ENOPROTOOPT)
		{
			std::cerr << "SO_ACCEPTCONN not supported by this protocol." << std::endl;
		}
		else if (errno == EINVAL)
		{
			std::cerr << "Socket is not valid or is not in a listening state." << std::endl;
		}
		return false;
	}
	return listening != 0;
}

bool checkSocket(int fd)
{
	if (!is_socket_bound(fd))
	{
		std::cerr << "Socket FD: " << fd << " is not bound." << std::endl;
		return false;
	}
	if (!is_socket_listening(fd))
	{
		std::cerr << "Socket FD: " << fd << " is not in a listening state." << std::endl;
		return false;
	}
	return true;
}