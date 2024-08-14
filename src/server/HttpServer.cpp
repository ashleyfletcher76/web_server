#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath, Logger &loggerRef) : config(confpath), logger(loggerRef)
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
	for (auto &entry : servers)
	{
		delete entry.second; // Free the memory allocated for each Server*
	}
	servers.clear();
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
	logger.logMethod("INFO", "Kqueue created successfully", NOSTATUS);

	for (const auto &srv : serverInfos)
	{
		try
		{
			Server *server = new Server(srv, logger);
			server->setKqueueEvent(kq);

			auto result = servers.emplace(server->getSocket(), server);
			if (!result.second)
			{
				throw std::runtime_error("Duplicate server port: " + std::to_string(srv.listen));
			}

			logger.logMethod("INFO", "Server is listening on port: " + std::to_string(srv.listen), NOSTATUS);
		}
		catch (const std::exception &e)
		{
			logger.logMethod("ERROR", "Failed to initialize server: " + std::string(e.what()), NOSTATUS);
		}
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

			std::cout << "event ident = " << event.ident << '\n';
			if (event.flags & EV_EOF)
			{
				logger.logMethod("INFO", "Connection closed by client: " + std::to_string(event.ident), NOSTATUS);
				closeSocket(event.ident);
				clientInfoMap.erase(event.ident);
				openSockets.erase(event.ident);
			}
			else if (event.filter == EVFILT_READ)
			{
				logger.logMethod("INFO", "Ready to read from FD: " + std::to_string(event.ident), NOSTATUS);

				auto serverIt = servers.find(static_cast<int>(event.ident));
				if (serverIt != servers.end())
				{
					logger.logMethod("INFO", "New connection on server FD: " + std::to_string(event.ident), NOSTATUS);
					acceptConnection(serverIt->second->getSocket());
				}
				else
				if (!isServerSocket)
				{
					logger.logMethod("INFO", "Reading request from FD: " + std::to_string(event.ident), NOSTATUS);
					readRequest(static_cast<int>(event.ident));
					handleRequest(static_cast<int>(event.ident));
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				logger.logMethod("INFO", "Ready to write to FD: " + std::to_string(event.ident), NOSTATUS);
				writeResponse(static_cast<int>(event.ident));
			}
		}
	}
}
