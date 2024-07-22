#include "HttpServer.hpp"

// Constructors
HttpServer::HttpServer(int port) : port(port), addrelen(sizeof(address)) {}

HttpServer::~HttpServer()
{
	close(new_socket);
	close(server_fd);
}

void	HttpServer::begin()
{
	init();
	bindSocket();
	startListening();
	while (true)
	{
		acceptConnection();
		readRequest();
		sendResponse();
		close(new_socket);
	}
}

void	HttpServer::init()
{
	//Create socket FD
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	HttpServer::bindSocket()
{
	//Bind socket to Network and Port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	HttpServer::startListening()
{
	//start listening
	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "Listen failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Server is listening on PORT " << port << std::endl;
}

void	HttpServer::acceptConnection()
{
	//accept incoming connection
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrelen)) < 0)
	{
		std::cerr << "Accept failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	HttpServer::readRequest()
{
	//read request
	char buffer[30000] = {0};
	read(new_socket, buffer, 30000);
	std::cout << "Recieved request:\n" << buffer << std::endl;

	//parse request to get path
	std::istringstream requestStream(buffer);
	std::string method;
	std::string path;
	requestStream >> method >> path;
	if (path == "/profile")
		requestedPath = "/profile.html";  // Correct file path
	else if (path == "/")
		requestedPath = "html/index.html";
	else
		requestedPath = "html" + path;  // Default mapping
}

void	HttpServer::sendResponse()
{
	//Send response
	std::string response;
	std::string content;


	std::cout << "Path = " << requestedPath << std::endl;
	if (requestedPath == "/profile")
	{
		std::cout << "Requested by me" << std::endl;
		content = readFileContent("../html/profile.html");
	}
	else if(requestedPath == "/")
		content = readFileContent("html/index.html");
	else if (requestedPath == "/favicon.ico")
	{
		// Respond with 204 No Content for favicon.ico
		response = "HTTP/1.1 204 No Content\n\n";
		write(new_socket, response.c_str(), response.length());
		std::cout << "Favicon request ignored\n";
		return ;
	}
	else
		content = "";
	if (!content.empty())
		response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
		+ std::to_string(content.length()) + "\n\n" + content;
	else
		response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 Not Found";
	write(new_socket, response.c_str(), response.length());
	std::cout << "Response sent\n";
}

std::string HttpServer::readFileContent(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file)
		return ("");
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}
