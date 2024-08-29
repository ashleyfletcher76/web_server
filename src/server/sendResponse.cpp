#include "HttpServer.hpp"

void HttpServer::writeResponse(int client_socket)
{
	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to write to non-existent client socket : " + std::to_string(client_socket));
		return;
	}

	std::string &response = clientResponse[client_socket];
	ssize_t totalBytesSent = 0;
	ssize_t bytesToSend = response.size();

	while (totalBytesSent < bytesToSend)
	{
		ssize_t bytesSent = send(client_socket, response.c_str() + totalBytesSent, bytesToSend - totalBytesSent, 0);
		if (bytesSent < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				logger.logMethod("ERROR", "Error writing to socket: " + std::string(strerror(errno)));
				deregisterWriteEvent(client_socket);
				closeSocket(client_socket);
				return;
			}
		}
		else if (bytesSent == 0)
		{
			logger.logMethod("INFO", "Client closed connection before the full response was sent.");
			deregisterWriteEvent(client_socket);
			closeSocket(client_socket);
			return;
		}
		totalBytesSent += bytesSent;
	}

	logger.logMethod("INFO", "Response successfully sent to FD: " + std::to_string(client_socket));

	if (!clientInfoMap[client_socket].shouldclose || clientInfoMap[client_socket].error)
	{
		if (openSockets.find(client_socket) != openSockets.end())
		{
			deregisterWriteEvent(client_socket);
			registerReadEvent(client_socket);
		}
		else
		{
			logger.logMethod("WARNING", "Socket already closed or removed from open sockets when trying to modify event.");
		}
	}
	else
	{
		logger.logMethod("INFO", "Closing socket because the connection type is close or error errror!");
		shutdown(client_socket, SHUT_WR);
		deregisterWriteEvent(client_socket);
		closeSocket(client_socket);
	}
}