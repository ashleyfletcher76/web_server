#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>

class HttpServer
{
	private:
		// variables;
		int			server_fd;
		int			new_socket;
		const int	port;
		int			addrelen;
		std::string	requestedPath;

		struct sockaddr_in	address;

		// methods
		void	init();
		void	bindSocket();
		void	startListening();
		void	acceptConnection();
		void	readRequest();
		void	sendResponse();
		std::string readFileContent(const std::string& filePath);

		// GET
		void	handleGetRequest(const std::string& path);

		// POST
		void	handlePostRequest(const std::string& path);

		// DELETE
		void	handleDeleteRequest(const std::string& path);


		// Error
		void		sendErrorResponse(int statusCode, const std::string& reasonPhrase);
		std::string	getErrorFilePath(int statusCode);

	public:
		HttpServer(int port);
		~HttpServer();

		void	begin();
};

#endif
