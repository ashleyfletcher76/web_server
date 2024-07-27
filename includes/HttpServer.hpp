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
		std::unordered_map<int, std::string> clients;
		std::vector<struct pollfd> &poll_fds;

		struct ClientInfo {
			std::string method;
			std::string requestedPath;
			bool responseReady;
		};

		std::unordered_map<int, ClientInfo> clientInfoMap;

		// methods
		void	init();
		void	bindSocket();
		void	startListening();
		void	acceptConnection();
		void	readRequest(int client_socket);
		void	sendResponse(int client_socket);
		std::string readFileContent(const std::string& filePath);

		// GET
		void	handleGetRequest(const std::string& path, int client_socket);

		// POST
		void	handlePostRequest(const std::string& path, int client_socket);

		// DELETE
		void	handleDeleteRequest(const std::string& path, int client_socket);


		// Error
		void		sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase);
		std::string	getErrorFilePath(int statusCode);

	public:
		HttpServer(int port, std::vector<struct pollfd> &poll_fds);
		~HttpServer();

		void	begin();
};

#endif
