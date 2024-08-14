#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath, Logger& loggerRef) : config(confpath), logger(loggerRef)
{
	init();
	mainLoop();
}

HttpServer::~HttpServer()
{
	logger.logMethod("INFO", "Shutting down server..", NOSTATUS);
	for (std::unordered_map<int, ClientInfo>::iterator it; it != clientInfoMap.end(); it++)
	{
		close(it->first);
	}
	if (openSockets.empty())
		std::cout << "No sockets left open" << std::endl;
	else
		std::cout << "Socket left open" << std::endl;
	pid_t pid = getpid();
	std::string command = "./utils/check_open_fds.sh " + std::to_string(pid);
	system(command.c_str());
}

void HttpServer::init()
{
	pid_t pid = getpid();
	std::cout << "MY PID is: " << pid << std::endl;
	kq = kqueue();
	if (kq == -1)
	{
		throw std::runtime_error("Kqueue creation failed: " + std::string(strerror(errno)));
	}
	for (auto &srv : serverInfos)
	{
		servers.emplace_back(srv, logger);
		servers.back().setKqueueEvent(kq);
	}
}

void printKevent(const struct kevent &event)
{
	std::cout << "kevent Details:" << std::endl;
	std::cout << "  ident:   " << event.ident << std::endl;
	std::cout << "  filter:  " << event.filter << std::endl;
	std::cout << "  flags:   " << event.flags << std::endl;
	std::cout << "  fflags:  " << event.fflags << std::endl;
	std::cout << "  data:    " << event.data << std::endl;
	std::cout << "  udata:   " << event.udata << std::endl;
}

void HttpServer::mainLoop()
{
	struct kevent events[1024];
	logger.logMethod("INFO", "Main loop started.", NOSTATUS);
	for (auto &srv : servers)
	{
		logger.logMethod("INFO", "Server is listening to : " + std::to_string(srv.getserverInfo().listen), NOSTATUS);
	}

	while (!shutdownFlag)
	{
		struct timespec timeout = {1, 0}; // Timeout of 1 second
		int nev = kevent(kq, NULL, 0, events, 1024, &timeout);
		if (nev < 0)
		{
			logger.logMethod("ERROR", "Error on kevent wait: " + std::string(strerror(errno)), NOSTATUS);
			continue;
		}
		for (int i = 0; i < nev; ++i)
		{
			struct kevent &event = events[i];
			//printKevent(event);
			logger.logMethod("INFO", "Event received: " + std::to_string(event.filter), NOSTATUS);

			if (event.flags & EV_EOF)
			{
				logger.logMethod("INFO", "Connection closed by client: " + std::to_string(event.ident), NOSTATUS);
				closeSocket(event.ident);
				clientInfoMap.erase(event.ident);
			}
			else if (event.filter == EVFILT_READ)
			{
				logger.logMethod("INFO", "Ready to read from FD: " + std::to_string(event.ident), NOSTATUS);

				bool isServerSocket = false;
				for (auto &srv : servers)
				{
					if (event.ident == srv.getSocket())
					{
						logger.logMethod("INFO", "New connection on server FD: " + std::to_string(event.ident), NOSTATUS);
						acceptConnection(srv.getSocket());
						isServerSocket = true;
						break;
					}
				}
				if (!isServerSocket)
				{
					logger.logMethod("INFO", "Reading request from FD: " + std::to_string(event.ident), NOSTATUS);
					readRequest(event.ident);
					handleRequest(event.ident);
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				logger.logMethod("INFO", "Ready to write to FD: " + std::to_string(event.ident), NOSTATUS);
				writeResponse(event.ident);
			}
		}
	}
}
