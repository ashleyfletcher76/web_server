#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath) : config(confpath)
{
	init();
	mainLoop();
}

HttpServer::~HttpServer()
{
	log("INFO", "Shutting down server..", NOSTATUS);
	std::ofstream logFile("log.txt", std::ios::app);
	if (logFile.is_open())
		logFile.close();
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
	std::cout << "Running shell script to check open fd's..." << std::endl;
	system(command.c_str());
}

void HttpServer::init()
{
	kq = kqueue();
	if (kq == -1)
	{
		throw std::runtime_error("Kqueue creation failed: " + std::string(strerror(errno)));
	}
	for (auto &srv : serverInfos)
	{
		servers.emplace_back(srv);
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
	log("INFO", "Main loop started.", NOSTATUS);
	for (auto &srv : servers)
	{
		log("INFO", "Server is listening to : " + std::to_string(srv.getserverInfo().listen), NOSTATUS);
	}

	while (!shutdownFlag)
	{
		struct timespec timeout = {1, 0}; // Timeout of 1 second
		int nev = kevent(kq, NULL, 0, events, 1024, &timeout);
		if (nev < 0)
		{
			log("ERROR", "Error on kevent wait: " + std::string(strerror(errno)), NOSTATUS);
			continue;
		}

		for (int i = 0; i < nev; ++i)
		{
			struct kevent &event = events[i];
			printKevent(event);
			log("INFO", "Event received: " + std::to_string(event.filter), NOSTATUS);

			if (event.flags & EV_EOF)
			{
				log("INFO", "Connection closed by client: " + std::to_string(event.ident), NOSTATUS);
				closeSocket(event.ident);
				clientInfoMap.erase(event.ident);
			}
			else if (event.filter == EVFILT_READ)
			{
				log("INFO", "Ready to read from FD: " + std::to_string(event.ident), NOSTATUS);

				bool isServerSocket = false;
				for (auto &srv : servers)
				{
					if (event.ident == srv.getSocket())
					{
						log("INFO", "New connection on server FD: " + std::to_string(event.ident), NOSTATUS);
						acceptConnection(srv.getSocket());
						isServerSocket = true;
						break;
					}
				}

				if (!isServerSocket)
				{
					log("INFO", "Reading request from FD: " + std::to_string(event.ident), NOSTATUS);
					readRequest(event.ident);
					handleRequest(event.ident);
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				log("INFO", "Ready to write to FD: " + std::to_string(event.ident), NOSTATUS);
				writeResponse(event.ident);
			}
		}
	}
}
