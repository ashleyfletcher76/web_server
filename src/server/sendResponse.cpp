#include "HttpServer.hpp"

void	HttpServer::writeResponse(int client_socket)
{
	std::string& response = clientInfoMap[client_socket].response;

	if (send(client_socket, response.c_str(), response.size(), 0) < 0)
		log("ERROR", "Error writing to socket: " + std::string(strerror(errno)), client_socket);

	close(client_socket);
	clientInfoMap.erase(client_socket);
}

