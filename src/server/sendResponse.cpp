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
		close(client_socket);
		clientInfoMap.erase(client_socket);
	}
}

