
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered timer event for socket: " + std::to_string(clientSocket));
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered timer event for socket: " + std::to_string(clientSocket));
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 5000, NULL); // 5 seconds timeout
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered timer event for socket: " + std::to_string(clientSocket));
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 5000, NULL); // 5 seconds timeout
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered timer event for socket: " + std::to_string(clientSocket));
	}
}
