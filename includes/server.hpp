#ifndef SERVER_HPP
#define SERVER_HPP

#include "config.hpp"
#include "includes.hpp"
#include "log.hpp"

class Server
{
private:
	serverInfo			info;
	uintptr_t			server_fd;
	struct sockaddr_in	address;
	Logger&					logger;

public:
	Server(const serverInfo &srinfo, Logger& loggerRef);
	~Server();
	int					_kq;

	void		createSocket();
	void		bindSocket();
	void		startListening();
	void		setKqueueEvent(int kq);
	uintptr_t	getSocket() const;
	serverInfo	getserverInfo() const;
};

#endif
