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
	try {
		bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	}
	catch(const std::exception& e)
	{
		std::cerr << "Bind failed! " << e.what() << std::endl;
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

void	HttpServer::sendResponse()
{
	//Send response
	std::string response;
	std::string content;

	content = readFileContent(requestedPath);
	if (!content.empty())
	{
		response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
	}
	else
		sendErrorResponse(404, "Not Found");
	write(new_socket, response.c_str(), response.length());
}
