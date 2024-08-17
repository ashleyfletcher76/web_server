#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include "server.hpp"
#include "log.hpp"
#include "database.hpp"

class HttpServer : public config
{
	private:
		// variables;
		std::unordered_map<int, Server*> servers;
		int			kq;

		struct sockaddr_in	address;

		std::unordered_map<int, std::string> clients;
		std::unordered_map<int, ClientInfo> clientInfoMap;
		std::set<int> openSockets;

		Logger& logger;
		Database& database;

		// methods
		void	init();
		void	mainLoop();

		// connection handlers
		void	acceptConnection(int serverSocket);
		void	setupKevent(int client_socket);
		void	configureSocketNonBlocking(int client_socket);
		void	closeSocket(int client_socket);
		void	modifyEvent(int fd, int filter, int flags);
		void	logSocketAction(const std::string& action, int fd);

		// request
		void	readRequest(int client_socket);
		void	handleRequest(int client_Socket);

		// content
		void	writeResponse(int client_socket);
		std::string getFilePath(const std::string& uri);
		std::string readFileContent(const std::string& filePath);

		// response
		bool	parseHttpRequest(const std::string& requestStream, HttpRequest& request, int client_socket);
		std::string formatHttpResponse(int status_code, const std::string& reasonPhrase,
		const std::string& body, int keepAlive);

		// GET
		void	handleGetRequest(int client_socket);

		// DELETE
		void	handleDeleteRequest(const std::string& path, int client_socket);

		// POST
		void	handlePostRequest(int client_socket);
		std::string	urlDecode(const std::string& str);
		std::string	sendResponsePost(int client_socket, ClientInfo &clientInfo);

		// Error
		void		sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase);
		std::string	getErrorFilePath(int statusCode, int serverFd);

		// Utils
		void	trim(std::string& str);


	public:
		HttpServer(std::string confpath, Logger& loggerRef, Database& databaseRef);
		~HttpServer();
};

#endif
