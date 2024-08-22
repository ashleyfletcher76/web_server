#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath, Logger& loggerRef, Database& databaseRef) : config(confpath),
	logger(loggerRef), database(databaseRef)
{
	//database.createTable();
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
	int	j = 1;

	while (!shutdownFlag)
	{
		struct timespec timeout = {1, 0}; // Timeout of 1 second
		int nev = kevent(kq, NULL, 0, events, 1024, &timeout);
		if (nev < 0)
		{
			logger.logMethod("ERROR", "Error on kevent wait: " + std::string(strerror(errno)));
			continue ;
		}
		for (int i = 0; i < nev; ++i)
		{
			struct kevent &event = events[i];
			logger.logMethod("INFO", "Event received: " + std::to_string(event.filter));
			if (event.flags & EV_EOF)
			{
				logger.logMethod("INFO", "Connection closed by client: " + std::to_string(event.ident));
				closeSocket(event.ident);
				clientInfoMap.erase(event.ident);
				openSockets.erase(event.ident);
			}
			if (event.filter == EVFILT_TIMER)
			{
				logger.logMethod("INFO", "Keep-alive timeout reached for socket: " + std::to_string(event.ident));
				closeSocket(event.ident);
				clientInfoMap.erase(event.ident);
				openSockets.erase(event.ident);
			}
			else if (event.filter == EVFILT_READ)
			{
				logger.logMethod("INFO", "Ready to read from FD: " + std::to_string(event.ident));

				auto serverIt = servers.find(static_cast<int>(event.ident));
				if (serverIt != servers.end())
				{
					logger.logMethod("INFO", "New connection on server FD: " + std::to_string(event.ident));
					acceptConnection(serverIt->second->getSocket());
				}
				else
				{
					logger.logMethod("INFO", "Reading request from FD: " + std::to_string(event.ident));
					readRequest(static_cast<int>(event.ident));
					if (clientInfoMap.find(event.ident) != clientInfoMap.end())
						handleRequest(event.ident); // check if socket is previously closed
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				logger.logMethod("INFO", "Ready to write to FD: " + std::to_string(event.ident));
				writeResponse(static_cast<int>(event.ident));
			}
			std::cout << "Num rotations: " + std::to_string(j) << std::endl;
			j++;
		}
	}
}
