#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(int port, std::vector<struct pollfd> &poll_fds) : port(port), addrelen(sizeof(address)), poll_fds(poll_fds) {}

HttpServer::~HttpServer()
{
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
	set_pollfd();
	mainLoop();
}

void HttpServer::init()
{
	// Create socket FD
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Socket creation failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		std::cerr << "setsockopt(SO_REUSEADDR) failed: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
	{
		std::cerr << "setsockopt(SO_REUSEPORT) failed: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
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
		std::cerr << "Bind failed: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void HttpServer::startListening()
{
	// start listening
	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "Listen failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server is listening on PORT " << port << std::endl;
}

void HttpServer::set_pollfd()
{
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	struct pollfd server_pollfd;
	server_pollfd.fd = server_fd;
	server_pollfd.events = POLLIN;

	poll_fds.push_back(server_pollfd);
}

void HttpServer::mainLoop()
{
	while (true)
	{
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
		if (poll_count < 0)
		{
			std::cerr << "Poll failed" << std::endl;
			exit(EXIT_FAILURE);
		}
		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents && POLLIN)
			{
				if (poll_fds[i].fd == server_fd)
					acceptConnection();
				else
					readRequest(poll_fds[i].fd);
			}
			if (poll_fds[i].revents & POLLOUT)
				sendResponse(poll_fds[i].fd);
		}
	}
}

void HttpServer::acceptConnection()
{
	struct sockaddr_in client_adress;
	socklen_t client_addrlen = sizeof(client_adress);
	int client_socket = accept(server_fd, (struct sockaddr *)&client_adress, &client_addrlen);
	if (client_socket < 0)
	{
		if (errno != EWOULDBLOCK && errno != EAGAIN)
		{
			std::cerr << "Accept failed" << std::endl;
			exit(EXIT_FAILURE);
		}
		return;
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd client_pollfd;
	client_pollfd.fd = client_socket;
	client_pollfd.events = POLLIN;
	poll_fds.push_back(client_pollfd);
	clientInfoMap[client_socket] = ClientInfo();
}
