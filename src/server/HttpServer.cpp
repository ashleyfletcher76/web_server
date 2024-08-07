#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(std::string confpath, int port, std::vector<struct pollfd> &poll_fds) : conf(config(confpath)), port(port), addrelen(sizeof(address)), poll_fds(poll_fds)
{
	conf.begin();
}

HttpServer::~HttpServer()
{
	log("INFO", "Shutting down server..", NOSTATUS);
	std::ofstream logFile("log.txt", std::ios::app);
	if (logFile.is_open())
		logFile.close();

	std::ifstream file("log.txt");
	if (file.good())
	{
		file.close();
		if (remove("log.txt") != 0)
			std::cerr << "Error deleting log file: " << strerror(errno) << std::endl;
		else
			std::cout << "Log file successfully deleted" << std::endl;
	}
	else
		std::cerr << "Log file does not exist or cannot be accessed" << std::endl;
	close(server_fd);
	for (std::unordered_map<int, ClientInfo>::iterator it; it != clientInfoMap.end(); it++)
	{
		close(it->first);
	}
}


void HttpServer::begin()
{
	init();
	bindSocket();
	startListening();
	setKqueueEvent();
	mainLoop();
}

void HttpServer::init()
{
	// Create socket FD
	kq = kqueue();
	if (kq == -1)
	{
		throw std::runtime_error("Kqueue creation failed: " + std::string(strerror(errno)));
	}
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
	}
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		throw std::runtime_error ("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)));
	}
}

void HttpServer::bindSocket()
{
	// Bind socket to Network and Port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
	}
}

void HttpServer::startListening()
{
	// start listening
	if (listen(server_fd, 3) < 0)
	{
		throw std::runtime_error ("Listen failed");
	}
	log("INFO", "Server is listening on PORT " + std::to_string(port), NOSTATUS);
}

void HttpServer::setKqueueEvent()
{
	struct kevent change;

	EV_SET(&change, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		throw std::runtime_error ("Kevent registration failure: " + std::string(strerror(errno)));
	}
}

void HttpServer::mainLoop()
{
	struct kevent event;
	log("INFO", "Main loop started.", NOSTATUS);
	while (!shutdownFlag)
	{
		struct timespec timeout = {1, 0};
		int nev = kevent(kq, NULL, 0, &event, 1, &timeout);
		if (nev < 0)
		{
			log("ERROR", "Error on kevent wait: " + std::string(strerror(errno)), NOSTATUS);
			continue ;
		}
		else if (nev > 0)
		{
			log("INFO", "Event received: " + std::to_string(event.filter), NOSTATUS);
			if (event.flags & EV_EOF)
			{
				log("INFO", "Connection closed by client: " + std::to_string(event.ident), NOSTATUS);
				close(event.ident);
				clientInfoMap.erase(event.ident);
			}
			else if (event.filter == EVFILT_READ)
			{
				log("INFO", "Ready to read from FD: " + std::to_string(event.ident), NOSTATUS);
				if (event.ident == server_fd)
				{
					log("INFO", "New connection on server FD", NOSTATUS);
					acceptConnection();
				}
				else
				{
					log("INFO", "Reading request from FD: " + std::to_string(event.ident), NOSTATUS);
					readRequest(event.ident);
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				log("INFO", "Ready to write to FD: " + std::to_string(event.ident), NOSTATUS);
			}
		}
	}

}

void HttpServer::acceptConnection()
{
	struct sockaddr_in client_address;
	socklen_t client_addrlen = sizeof(client_address);
	int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
	if (client_socket < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error("Accept failed: " + std::string(strerror(errno)));
		return ;
	}
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
	log("INFO", "Accepted connection from IP: " + std::string(client_ip) + " on socket: " + std::to_string(client_socket), NOSTATUS);
	fcntl(client_socket, F_SETFL, O_NONBLOCK); // sets to non blocking mode
	struct kevent change;
	EV_SET(&change, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
	{
		std::string error_msg = "Kevent registration failed: " + std::string(strerror(errno));
		throw std::runtime_error(error_msg);
	}
	clientInfoMap[client_socket] = ClientInfo();
}
