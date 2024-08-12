#include "HttpServer.hpp"

void	HttpServer::writeResponse(int client_socket)
{
	std::string& response = clientInfoMap[client_socket].response;

	// sends reponse to client(web browser etc.)
	if (send(client_socket, response.c_str(), response.size(), 0) < 0)
		log("ERROR", "Error writing to socket: " + std::string(strerror(errno)), client_socket);

	// only closed if not keep-alive in HTTP response
	if (clientInfoMap[client_socket].shouldclose)
	{
		//close(client_socket);
		closeSocket(client_socket);
		clientInfoMap.erase(client_socket);
	}
	else
	{
		struct kevent change;
		EV_SET(&change, client_socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
			log("ERROR", "Failed to remove write event for FD: " + std::to_string(client_socket), NOSTATUS);
	}
}

