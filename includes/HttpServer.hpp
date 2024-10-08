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
		std::unordered_map<int, std::chrono::steady_clock::time_point> socket_last_activity;
		const std::chrono::seconds idle_timeout = std::chrono::seconds(10);
		struct sockaddr_in	address;
		std::unordered_map<int, ClientInfo> clientInfoMap;
		std::unordered_map<int, std::string> clientResponse;
		std::set<int> openSockets;

		Logger& logger;
		Database& database;

		// methods
		void	init();
		void	mainLoop();

		// connection handlers
		void	acceptConnection(int serverSocket);
		void	configureSocketNonBlocking(int client_socket);
		void	closeSocket(int client_socket);
		void	modifyEvent(int fd, int filter, int flags);
		void	logSocketAction(const std::string& action, int fd);

		// request
		void	readRequest(int client_socket);
		bool	handleHeadersAndCheckForBody(int client_socket);
		bool	readFullRequestBody(int client_socket, std::string::size_type contentLengthPos);
		void	handleRequest(int client_Socket);
		bool	validateServer(int client_socket);
		bool	validateRouteAndMethod(int client_socket, HttpRequest &request);
		void	decideConnectionPersistence(int client_socket, const HttpRequest &request);
		void	processRequestMethod(int client_socket);

		// content
		void		writeResponse(int client_socket);
		std::string	getFilePath(int server_fd, std::string &uri);
		std::string	readFileContent(const std::string& filePath);

		// parse
		bool		parseHttpRequest(const std::string& requestStream, HttpRequest& request, int client_socket);
		bool		parseHttpRequestBody(std::istringstream& requestStream, HttpRequest& request, int client_socket);
		bool		parseHttpRequestHeaders(std::istringstream& requestStream, HttpRequest& request);
		std::string	extractHeaderValue(const std::string &headers, const std::string &key);
		std::string	extractFilename(const std::string &contentDisposition);
		void 		parseMultipartBody(const std::string &body, const std::string &boundary, HttpRequest &request);
		std::string	extractBoundary(const std::string &contentType);

		//register events
		void	deregisterChild(int clientSocket, pid_t pid);
		void	deregisterWriteEvent(int clientSocket);
		void	deregisterReadEvent(int clientSocket);
		void	deregisterTimer(int clientSocket);
		void	registerReadEvent(int clientSocket);
		void	registerWriteEvent(int clientSocket);
		void	registerChild(int client_Socket, pid_t pid);

		// response
		void		sendRedirectResponse(int client_socket, const std::string &redirectUrl);
		std::string	formatHttpResponse(const std::string& httpVersion, int status_code, const std::string& reasonPhrase,
			const std::string& body, int keepAlive, const std::string& filePath);

		// GET
		void	handleGetRequest(int client_socket);

		// DELETE
		void	handleDeleteRequest(int client_socket, HttpRequest &request);

		// POST
		void	handlePostRequest(int client_socket);
		void	handleSubmitForm(int client_socket, HttpRequest &request);
		void	handleUpload(int client_socket, HttpRequest &request);
		std::map<std::string, std::string>	parseFormData(const std::string& body);

		// CGI
		void	setupCgiEnvironment(int client_socket);
		bool	checkIfCgiAllowed(const std::string& uri, int client_socket, const serverInfo& srvInfo);
		std::string	findHandler(const std::string& uri, int client_socket);
		std::string	parseCgiOutput(std::string cgiOutput);
		void	executeCGI(const std::string& scriptPath, int client_Socket, const std::vector<std::string>& envp);
		void	executeCGI_Event(struct kevent &event);

		//directory listing
		void	handleDirectoryListing(int client_socket, const std::string &dirPath);
		bool	isDirectory(const std::string &path);
		bool	fileExists(const std::string &path);
		std::vector<std::string> listDirectory(const std::string &directoryPath);

		// Error
		void		sendErrorResponse(int client_socket, int statusCode, const std::string &reasonPhrase);
		std::string	getErrorFilePath(int statusCode, int serverFd);
		std::string replacePlaceholder(const std::string &content, const std::string &placeholder, const std::string &value);

		// Generate custom Pages
		std::string	generateProfilePage(const userProfile& profile);
		void		generateAllProfilesPage(int client_socket);
		bool		findProfileByID(const std::string& uri, int client_socket);
		void		uploadsPage(int client_socket);
		void		serveFile(int client_socket, const std::string &uri);

		// Utils
		void	trim(std::string& str);
		int		getMaxClientBodySize(int client_socket);


	public:
		HttpServer(std::string confpath, Logger& loggerRef, Database& databaseRef);
		~HttpServer();
};

#endif
