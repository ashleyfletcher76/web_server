#include "HttpServer.hpp"

std::string HttpServer::formatHttpResponse(int status_code, const std::string& reasonPhrase,
	const std::string& body)
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

void	HttpServer::closeSocket(int client_socket)
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

std::string HttpServer::getFilePath(const std::string& uri)
{
	return ("html" + uri);
}
