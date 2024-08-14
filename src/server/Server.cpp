#include "server.hpp"

Server::Server(const serverInfo &srinfo, Logger &loggerRef) : info(srinfo), server_fd(-1),
															  logger(loggerRef), _kq(-1)
{
	createSocket();
}

Server::~Server()
{
	close(server_fd);
}

void Server::createSocket()
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
	}

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		throw std::runtime_error("Failed to set non-blocking mode for socket: " + std::string(strerror(errno)));
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		close(server_fd);
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)));
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(info.listen);
	address.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all interfaces

	bindSocket();
	startListening();

	logger.logMethod("INFO", "Server socket setup correctly for port: " + std::to_string(info.listen), NOSTATUS);
}

void Server::bindSocket()
{
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(server_fd);
		throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
	}
	logger.logMethod("INFO", "binding correctly seted up for " + std::to_string(info.listen), NOSTATUS);
}

void Server::startListening()
{
	if (server_fd < 0)
	{
		throw std::runtime_error("Invalid socket file descriptor.");
	}
	if (listen(server_fd, SOMAXCONN) == -1)
	{
		close(server_fd);
		throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
	}
	logger.logMethod("INFO", "listen correctly seted up for " + std::to_string(info.listen), NOSTATUS);
}

void Server::setKqueueEvent(int kq)
{
	this->_kq = kq;
	struct kevent event;
	EV_SET(&event, static_cast<uintptr_t>(server_fd), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1)
	{
		throw std::runtime_error("Kevent setup failed: " + std::string(strerror(errno)));
	}
	logger.logMethod("INFO", "Kevent correctly set up for " + std::to_string(info.listen), NOSTATUS);
}

int Server::getSocket() const { return server_fd; }

serverInfo Server::getserverInfo() const { return info; }
