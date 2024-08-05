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
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <unordered_map>
#include <limits.h>
#include <sys/wait.h>


struct ClientInfo {
	std::string	method;
	std::string	requestedPath;
	std::string postData;
	bool		responseReady;
};


#include "config.hpp";

class HttpServer
{
	private:
		// variables;
		const int	port;
		int			server_fd;
		int			new_socket;
		int			addrelen;

		struct sockaddr_in	address;

		std::vector<struct pollfd> &poll_fds;
		std::unordered_map<int, std::string> clients;
		std::unordered_map<int, ClientInfo> clientInfoMap;

		// methods
		void	init();
		void	mainLoop();
		void	set_pollfd();
		void	bindSocket();
		void	startListening();
		void	acceptConnection();


		void	readRequest(int client_socket);
		void	sendResponse(int client_socket);

		std::string readFileContent(const std::string& filePath);

		// GET
		void	handleGetRequest(const std::string& path, int client_socket);

		// DELETE
		void	handleDeleteRequest(const std::string& path, int client_socket);

		// POST
		void	handlePostRequest(const std::string &path, int client_socket, char buffer[30000]);
		std::string	sendResponsePost(int client_socket, ClientInfo &clientInfo);

		// Error
		void		sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase);
		std::string	getErrorFilePath(int statusCode);

	public:
		HttpServer(int port, std::vector<struct pollfd> &poll_fds);
		~HttpServer();

		void	begin();
};

#endif
