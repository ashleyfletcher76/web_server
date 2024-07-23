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
		requestedPath = "/Users/asfletch/Desktop/working_on/web_server/html/profile.html";  // Correct file path
	else if (path == "/")
		requestedPath = "/Users/asfletch/Desktop/working_on/web_server/html/index.html";
	else
		requestedPath = "html" + path;  // Default mapping
}

void	HttpServer::sendResponse()
{
	//Send response
	std::string response;
	std::string content;

	std::cout << "Path = " << requestedPath << std::endl;
	content = readFileContent(requestedPath);
	if (!content.empty())
	{
		response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
		std::cout << "Repsonse sent with content length: " << content.length() << std::endl;
	}
	else
	{
		response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 Not Found";
		std::cout << "File not found, 404 response" << std::endl;
	}
	write(new_socket, response.c_str(), response.length());
	std::cout << "Response sent\n";
}

std::string getCurrentWorkingDirectory()
{
	char	temp[PATH_MAX];
	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string HttpServer::readFileContent(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		std::cerr << "Current working directory: " << getCurrentWorkingDirectory() << std::endl;
		return ("");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

