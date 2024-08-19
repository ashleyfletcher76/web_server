#include "HttpServer.hpp"

std::string HttpServer::formatHttpResponse(int status_code, const std::string &reasonPhrase,
										   const std::string &body, int keepAlive)
{
	std::ostringstream response;

	// constructs proper format for HTTP response
	response << "HTTP/1.1 " << status_code << " " << reasonPhrase << "\r\n";
	response << "Content-Length: " << body.size() << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	if (keepAlive)
		response << "Connection: close\r\n";
	else
		response << "Connection: keep-alive\r\n";
	response << "\r\n";
	response << body;
	return (response.str());
}

void HttpServer::modifyEvent(int fd, int filter, int flags)
{
	logSocketAction("Modifying event", fd);
	if (openSockets.find(fd) == openSockets.end())
	{
		logger.logMethod("WARNING", "Attempt to modify event for closed or non-existent FD: " + std::to_string(fd));
		return;
	}
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(fd), filter, flags, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
		logger.logMethod("ERROR", "Failed to modify event: (" + std::to_string(flags) + ") " + std::string(strerror(errno)) + " for FD: " + std::to_string(fd));
	else
		logger.logMethod("INFO", "Successfully modified event for FD: " + std::to_string(fd));
}

void HttpServer::logSocketAction(const std::string &action, int fd)
{
	std::stringstream ss;
	ss << "Socket FD: " << fd << " Action: " << action << ". Open sockets count: " << openSockets.size();
	logger.logMethod("DEBUG", ss.str());
}

void HttpServer::closeSocket(int client_socket)
{
	if (openSockets.find(client_socket) == openSockets.end())
	{
		logger.logMethod("WARNING", "Attempted to close an already closed or non-existent FD: " + std::to_string(client_socket));
		return;
	}
	modifyEvent(client_socket, EVFILT_READ, EV_DELETE);
	modifyEvent(client_socket, EVFILT_WRITE, EV_DELETE);
	close(client_socket);
	openSockets.erase(client_socket);
	auto iter = clientInfoMap.find(client_socket);
	if (iter != clientInfoMap.end())
	{
		delete iter->second;
		clientInfoMap.erase(client_socket);
		logger.logMethod("INFO", "Closed client socket FD: " + std::to_string(client_socket));
		logSocketAction("Closed", client_socket);
	}
	else
		logger.logMethod("WARNING", "Client socket FD not found in clientInfoMap: " + std::to_string(client_socket));
}

std::string HttpServer::getFilePath(int server_fd, const std::string &uri)
{
	auto serverIt = servers.find(server_fd);
	if (serverIt == servers.end())
	{
		return "";
	}

	const serverInfo &srv = serverIt->second->getServerInfo();
	for (const auto &routePair : srv.routes)
	{
		const std::string &routePath = routePair.first;
		const routeConfig &route = routePair.second;
		if (uri.find(routePath) == 0)
		{
			if (!route.redirect.empty())
			{
				return route.redirect;
			}

			std::string filePath = srv.document_root + uri;

			if (uri == "/")
			{
				filePath += srv.default_file;
			}
			return filePath;
		}
	}

	std::string defaultPath = srv.document_root + uri;

	if (uri.back() == '/')
	{
		defaultPath += srv.default_file;
	}

	if (access(defaultPath.c_str(), F_OK) != -1)
	{
		return defaultPath;
	}
	return getErrorFilePath(404, server_fd);
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
