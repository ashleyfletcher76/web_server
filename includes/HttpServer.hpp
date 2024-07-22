#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


class HttpServer
{
	private:
		// variables;
		int			server_fd;
		int			new_socket;
		const int	port;
		int			addrelen;

		struct sockaddr_in	address;

		// methods
		void	init();
		void	bindSocket();
		void	startListening();
		void	acceptConnection();
		void	readRequest();
		void	sendResponse();

	public:
		HttpServer(int port);
		~HttpServer();

		void	begin();
};

#endif
