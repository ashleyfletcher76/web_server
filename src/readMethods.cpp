#include "HttpServer.hpp"

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
	if (method == "GET")
		handleGetRequest(path);
	else
		sendErrorResponse(405, "Method Not Allowed");
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
