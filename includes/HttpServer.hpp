#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include "Server.hpp"

class HttpServer : public config
{
	private:
		// variables;
		uintptr_t	server_fd;
		std::vector<Server> servers;
		int			new_socket;
		int			kq;

		struct sockaddr_in	address;

		std::unordered_map<int, std::string> clients;
		std::unordered_map<int, ClientInfo> clientInfoMap;
		std::set<int> openSockets;

		// methods
		void	init();
		void	mainLoop();

		// connection handlers
		void	acceptConnection(int serverSocket);
		void	setupKevent(int client_socket);
		void	configureSocketNonBlocking(int client_socket);
		void	closeSocket(int client_socket);

		// request
		void	readRequest(int client_socket);
		void	handleRequest(int client_Socket);

		// content
		void	writeResponse(int client_socket);
		std::string getFilePath(const std::string& uri);
		std::string readFileContent(const std::string& filePath, int client_socket);

		// response
		bool	parseHttpRequest(const std::string& requesStr, HttpRequest& request);
		std::string formatHttpResponse(int status_code, const std::string& reasonPhrase, const std::string& body);

		// GET
		void	handleGetRequest(int client_socket);

		// DELETE
		void	handleDeleteRequest(const std::string& path, int client_socket);

		// POST
		void	handlePostRequest(const std::string &path, int client_socket, char buffer[30000]);
		std::string	sendResponsePost(int client_socket, ClientInfo &clientInfo);

		// Error
		void		sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase);
		std::string	getErrorFilePath(int statusCode);


	public:
		HttpServer(std::string confpath);
		~HttpServer();
};

#endif
