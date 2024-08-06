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

		std::string scriptPath = "./cgi" + clientInfo.requestedPath;

		std::vector<std::string> envp;
		envp.push_back("CONTENT_LENGTH=" + std::to_string(clientInfo.postData.size()));
		envp.push_back("REQUEST_METHOD=POST");
		envp.push_back("SCRIPT_NAME=" + scriptPath);
		envp.push_back("QUERY_STRING=");
		envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envp.push_back("GATEWAY_INTERFACE=CGI/1.1");

		std::vector<const char *> env;
		for (std::vector<std::string>::iterator it = envp.begin(); it != envp.end(); ++it)
		{
			env.push_back(it->c_str());
		}
		env.push_back(nullptr);
		const char *argv[] = {scriptPath.c_str(), nullptr};

		execve(scriptPath.c_str(), const_cast<char *const *>(argv), const_cast<char *const *>(env.data()));
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
			response = "HTTP/1.1 " + std::to_string(clientInfoMap[client_socket].statusCode) +
				" OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
			if (clientInfo.method == "GET")
				log ("INFO", "HTTP/1.1 " + std::to_string(clientInfoMap[client_socket].statusCode) + " OK - " " Recieved GET request: " + clientInfo.requestedPath + " OK\nContent-Type: text/html\nContent-Length: "
					+ std::to_string(content.length()) + "\n\n", client_socket);
			//std::cout << response << std::endl;
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
