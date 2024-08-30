#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath, Logger &loggerRef, Database &databaseRef) : config(confpath),
																						 logger(loggerRef), database(databaseRef)
{
	// database.createTable();
	(void)databaseRef;
	init();
	mainLoop();
}

HttpServer::~HttpServer()
{
	logger.logMethod("INFO", "Shutting down server..");
	std::cout << "Number socket still open: " << clientInfoMap.size() << std::endl;

	auto iter = clientInfoMap.begin();
	while (iter != clientInfoMap.end())
	{
		deregisterReadEvent(iter->first);
		closeSocket(iter->first);
		iter = clientInfoMap.begin();
	}
	if (openSockets.empty())
		std::cout << "No sockets left open" << std::endl;
	else
		std::cout << "Socket left open" << std::endl;
	for (auto &entry : servers)
	{
		close(entry.second->getSocket());
		delete entry.second; // Free the memory allocated for each Server*
	}
	servers.clear();
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
	logger.logMethod("INFO", "Kqueue created successfully");

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
			logger.logMethod("INFO", "Server is listening on port: " + std::to_string(srv.listen));
		}
		catch (const std::exception &e)
		{
			logger.logMethod("ERROR", "Failed to initialize server: " + std::string(e.what()));
		}
	}
}

void HttpServer::mainLoop()
{
	struct kevent events[1024];
	logger.logMethod("INFO", "Main loop started.");

	while (!shutdownFlag)
	{
		struct timespec timeout = {1, 0};
		int nev = kevent(kq, NULL, 0, events, 1024, &timeout);
		if (nev < 0)
		{
			continue;
		}
		for (int i = 0; i < nev; ++i)
		{
			struct kevent &event = events[i];
			int fd = static_cast<int>(event.ident);
			if (event.flags & EV_EOF)
			{
				switch (event.filter)
				{
				case EVFILT_READ:
					deregisterReadEvent(fd);
					closeSocket(fd);
					break;
				case EVFILT_WRITE:
					deregisterWriteEvent(fd);
					closeSocket(fd);
					break;
				case EVFILT_PROC:
					executeCGI_Event(event);
					break;
				default:
					break;
				}
				logger.logMethod("INFO", "Closing socket because of end of file send!");
				continue;
			}
			switch (event.filter)
			{
			case EVFILT_READ:
			{
				auto serverIt = servers.find(fd);
				if (serverIt != servers.end())
				{
					acceptConnection(serverIt->second->getSocket());
				}
				else
				{
					readRequest(fd);
				}
				break;
			}
			case EVFILT_WRITE:
				writeResponse(fd);
				break;
			case EVFILT_PROC:
				executeCGI_Event(event);
				break;
			case EVFILT_TIMER:
			{
				if (clientInfoMap[fd].outpipe != -1 || clientInfoMap[fd].pid != -1)
				{
					logger.logMethod("WARNING", "Child process timed out and was killed.");
					deregisterChild(fd, clientInfoMap[fd].pid);
					kill(clientInfoMap[fd].pid, SIGKILL);
					close(clientInfoMap[fd].outpipe);
					sendErrorResponse(fd, 504, "Gateway Timeout");
				}
				else
				{
					logger.logMethod("INFO", "Closing socket due to inactivity (timer event): " + std::to_string(fd));
					deregisterReadEvent(fd);
					closeSocket(fd);
				}
				break;
			}
			default:
				break;
			}
		}
	}
}
