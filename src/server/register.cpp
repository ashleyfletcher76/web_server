
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, idle_timeout.count() * 1000, NULL);
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

	EV_SET(&change, static_cast<uintptr_t>(clientSocket), EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, idle_timeout.count() * 1000, NULL); // 5 seconds timeout
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered timer event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::registerChild(int clientSocket, pid_t pid)
{
	struct kevent procEvent;
	EV_SET(&procEvent, pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, 0, reinterpret_cast<void *>(clientSocket));

	if (kevent(kq, &procEvent, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register child process event for PID: " + std::to_string(pid) + ", error: " + std::string(strerror(errno)));
		return;
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered child process event for PID: " + std::to_string(pid));
	}

	struct kevent timerEvent;
	EV_SET(&timerEvent, clientSocket, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, idle_timeout.count() * 1000, NULL);

	if (kevent(kq, &timerEvent, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to register timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
		deregisterChild(clientSocket, pid);
		return;
	}
	else
	{
		logger.logMethod("INFO", "Successfully registered timer event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::deregisterChild(int clientSocket, pid_t pid)
{
	struct kevent procEvent;
	EV_SET(&procEvent, pid, EVFILT_PROC, EV_DELETE, 0, 0, reinterpret_cast<void *>(clientSocket));

	if (kevent(kq, &procEvent, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister child process event for PID: " + std::to_string(pid) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered child process event for PID: " + std::to_string(pid));
	}

	struct kevent timerEvent;
	EV_SET(&timerEvent, clientSocket, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &timerEvent, 1, NULL, 0, NULL) == -1)
	{
		logger.logMethod("ERROR", "Failed to deregister timer event for socket: " + std::to_string(clientSocket) + ", error: " + std::string(strerror(errno)));
	}
	else
	{
		logger.logMethod("INFO", "Successfully deregistered timer event for socket: " + std::to_string(clientSocket));
	}
}

void HttpServer::deregisterTimer(int clientSocket)
{
	struct kevent change;
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