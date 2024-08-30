#include "HttpServer.hpp"

void HttpServer::writeResponse(int client_socket)
{
	if (clientInfoMap.find(client_socket) == clientInfoMap.end())
	{
		logger.logMethod("ERROR", "Attempt to write to non-existent client socket : " + std::to_string(client_socket));
		return;
	}

	std::string &response = clientResponse[client_socket];
	ssize_t &totalBytesSent = clientInfoMap[client_socket].totalBytesSent;
	ssize_t bytesToSend = response.size();

	ssize_t maxBytesPerWrite = 1024;
	ssize_t remainingBytes = bytesToSend - totalBytesSent;
	ssize_t bytesToSendNow = std::min(maxBytesPerWrite, remainingBytes);

	ssize_t bytesSent = send(client_socket, response.c_str() + totalBytesSent, bytesToSendNow, 0);
	if (bytesSent < 0)
	{
		logger.logMethod("WARNING", "Socket is tempraroaly unavailable.");
		return;
	}
	else if (bytesSent == 0)
	{
		logger.logMethod("WARNING", "Client closed connection before the full response was sent.");
		deregisterReadEvent(client_socket);
		deregisterWriteEvent(client_socket);
		closeSocket(client_socket);
		return;
	}

	totalBytesSent += bytesSent;

	if (totalBytesSent >= bytesToSend)
	{
		logger.logMethod("INFO", "Response successfully sent to FD: " + std::to_string(client_socket));

		if (!clientInfoMap[client_socket].shouldclose || clientInfoMap[client_socket].error)
		{
			if (openSockets.find(client_socket) != openSockets.end())
			{
				deregisterWriteEvent(client_socket);
			}
			else
			{
				logger.logMethod("WARNING", "Socket already closed or removed from open sockets when trying to modify event.");
			}
		}
		else
		{
			logger.logMethod("INFO", "Closing socket because the connection type is close or error occurred.");
			shutdown(client_socket, SHUT_WR);
			deregisterReadEvent(client_socket);
			deregisterWriteEvent(client_socket);
			closeSocket(client_socket);
		}

		// Reset the totalBytesSent for the next response
		totalBytesSent = 0;
	}
	else
	{
		// Not all bytes were sent, keep the write event registered
		logger.logMethod("INFO", "Partial response sent, awaiting further writes.");
	}
}