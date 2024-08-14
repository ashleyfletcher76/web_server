#include "HttpServer.hpp"

std::string HttpServer::formatHttpResponse(int status_code, const std::string& reasonPhrase,
	const std::string& body, int keepAlive)
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

void	HttpServer::modifyEvent(int fd, int filter, int flags)
{
	if (openSockets.find(fd) == openSockets.end())
	{
		logger.logMethod("WARNING", "Attempt to modify event for closed or non-existent FD: " + std::to_string(fd), NOSTATUS);
		return;
	}
	struct kevent change;
	EV_SET(&change, fd, filter, flags, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
		logger.logMethod("ERROR", "Failed to modify event: ("  + std::to_string(flags) + ") " + std::string(strerror(errno)) + " for FD: " + std::to_string(fd), NOSTATUS);
	else
		logger.logMethod("INFO", "Successfully modified event for FD: " + std::to_string(fd), NOSTATUS);
}

void	HttpServer::closeSocket(int client_socket)
{
	if (openSockets.find(client_socket) == openSockets.end())
	{
		logger.logMethod("WARNING", "Attempted to close an already closed or non-existent FD: " + std::to_string(client_socket), NOSTATUS);
		return;
	}
	modifyEvent(client_socket, EVFILT_READ, EV_DELETE);
	modifyEvent(client_socket, EVFILT_WRITE, EV_DELETE);
	close(client_socket);
	openSockets.erase(client_socket);
	clientInfoMap.erase(client_socket);
	logger.logMethod("INFO", "Closed client socket FD: " + std::to_string(client_socket), NOSTATUS);
}

std::string HttpServer::getFilePath(const std::string& uri)
{
	return ("html" + uri);
}
