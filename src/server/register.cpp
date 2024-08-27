
#include "HttpServer.hpp"

void HttpServer::deregisterReadEvent(int clientSocket)
{
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_READ, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister read event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered read event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::deregisterWriteEvent(int clientSocket)
{
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister write event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered write event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::registerWriteEvent(int clientSocket)
{
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register write event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered write event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::registerReadEvent(int clientSocket)
{
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register read event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered read event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::checkIdleSockets()
{
	auto now = std::chrono::steady_clock::now();

	for (auto it = socket_last_activity.begin(); it != socket_last_activity.end();)
	{
		auto socket_fd = it->first;
		auto last_activity_time = it->second;
		if (now - last_activity_time > idle_timeout)
		{
			logger.logMethod("INFO", "Closing socket because of idle timing!");
			deregisterReadEvent(socket_fd);
			closeSocketIdle(socket_fd);
			it = socket_last_activity.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void HttpServer::updateLastActivity(int socket_fd)
{
	auto serverIt = servers.find(socket_fd);
	if (serverIt != servers.end()) { return; }
	socket_last_activity[socket_fd] = std::chrono::steady_clock::now();
}
