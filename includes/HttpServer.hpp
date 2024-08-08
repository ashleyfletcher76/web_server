#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include "config.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <limits.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/event.h>
#include <sys/time.h>
#include <exception>
#include <ctime>
#include <iomanip>
#include <arpa/inet.h>
#include <csignal>
#include <cstdio>

#define NOSTATUS -5


// struct ClientInfo {
// 	std::string	method;
// 	std::string	requestedPath;
// 	std::string postData;
// 	bool		responseReady;
// 	int			statusCode;
// };

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string body;
	std::string version;
	std::map<std::string, std::string> headers;
};

struct HttpResponse
{
	int statusCode;
	std::string reason;
	std::string body;
	std::map<std::string, std::string> headers;
};

struct ClientInfo
{
	HttpRequest request;
	std::string response;
};

#include "config.hpp"

class HttpServer
{
	private:
		// variables;
		config		conf;
		const int	port;
		uintptr_t	server_fd;
		int			new_socket;
		int			addrelen;
		int			kq;

		struct sockaddr_in	address;

		std::vector<struct pollfd> &poll_fds;
		std::unordered_map<int, std::string> clients;
		std::unordered_map<int, ClientInfo> clientInfoMap;

		// methods
		void	init();
		void	mainLoop();
		void	setKqueueEvent();
		void	bindSocket();
		void	startListening();
		// connection handlers
		void	acceptConnection();
		void	setupKevent(int client_socket);
		void	configureSocketNonBlocking(int client_socket);


		void	readRequest(int client_socket);
		void	handleRequest(int client_Socket);

		void	writeResponse(int client_socket);
		std::string getFilePath(const std::string& uri);

		bool	parseHttpRequest(const std::string& requesStr, HttpRequest& request);
		std::string formatHttpResponse(int status_code, const std::string& reason_phrase, const std::string& body);

		std::string readFileContent(const std::string& filePath, int client_socket);

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

		//Log
		void	log(const std::string& level, const std::string& msg, int client_socket);

	public:
		HttpServer(std::string confpath, int port, std::vector<struct pollfd> &poll_fds);
		~HttpServer();
		config getConfig() {return(conf);}
		void	begin();
};

extern volatile sig_atomic_t shutdownFlag;
void	signalHandler(int signum);

#endif
