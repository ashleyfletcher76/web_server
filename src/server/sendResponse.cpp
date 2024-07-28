#include "HttpServer.hpp"

std::string HttpServer::sendResponsePost(int client_socket, ClientInfo &clientInfo)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		sendErrorResponse(client_socket, 500, "Internal Server Error");
		return "";
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		sendErrorResponse(client_socket, 500, "Internal Server Error");
		return "";
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);

		setenv("CONTENT_LENGTH", std::to_string(clientInfo.postData.size()).c_str(), 1);
		setenv("REQUEST_METHOD", "POST", 1);

		execl(clientInfo.requestedPath.c_str(), clientInfo.requestedPath.c_str(), NULL);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[1]);
		waitpid(pid, NULL, 0);

		char buffer[1024];
		std::ostringstream responseStream;
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		{
			responseStream.write(buffer, bytesRead);
		}
		close(pipefd[0]);
		return (responseStream.str());
	}
}

void HttpServer::sendResponse(int client_socket)
{
	// Send response
	ClientInfo &clientInfo = clientInfoMap[client_socket];


	if (clientInfo.responseReady)
	{
		std::cout << "Sending response for method: " << clientInfo.method << std::endl;
		std::cout << "Requested path: " << clientInfo.requestedPath << std::endl;
		std::cout << "Response ready: " << clientInfo.responseReady << std::endl;
		std::string response;
		std::string content;

		if (clientInfo.method == "GET")
		{
			content = readFileContent(clientInfo.requestedPath);
		}
		else if (clientInfo.method == "POST")
		{
			content = sendResponsePost(client_socket, clientInfo);
		}

		if (!content.empty())
		{
			response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
		}
		else
		{
			sendErrorResponse(client_socket, 404, "Not Found");
		}

		write(client_socket, response.c_str(), response.length());
		close(client_socket);
		clientInfoMap.erase(client_socket);
	}
}
