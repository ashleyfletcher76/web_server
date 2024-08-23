#include "HttpServer.hpp"

void HttpServer::writeResponse(int client_socket)
{
	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to write to non-existent client socket : " + std::to_string(client_socket));
		return;
	}
	std::string& response = clientResponse[client_socket];

	if (send(client_socket, response.c_str(), response.size(), 0) < 0)
	{
		logger.logMethod("ERROR", "Error writing to socket: " + std::string(strerror(errno)));
		closeSocket(client_socket);
		return;
	}
	else
		logger.logMethod("INFO", "Response successfully sent to FD: " + std::to_string(client_socket));

	if (!clientInfoMap[client_socket].shouldclose)
	{
		if (openSockets.find(client_socket) != openSockets.end())
			{
				deregisterWriteEvent(client_socket);
				registerReadEvent(client_socket);
			}
		else
			logger.logMethod("WARNING", "Socket already closed or removed from open sockets when trying to modify event.");
	}
	else
	{
		closeSocket(client_socket);
	}
}
