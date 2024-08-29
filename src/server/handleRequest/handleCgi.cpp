#include "HttpServer.hpp"

std::string	removeLeading(const std::string& path)
{
	if (!path.empty() && path[0] == '/')
		return (path.substr(1));
	return (path);
}

void	HttpServer::setupCgiEnvironment(int client_socket)
{
	auto& env = clientInfoMap[client_socket].cgiEnv;
	auto& request = clientInfoMap[client_socket].request;
	const auto port = serverInfos[client_socket].listen;

	if (!checkIfCgiAllowed(request.uri, client_socket))
	{
		logger.logMethod("ERROR", "CGI script not allowed: " + request.uri);
		return ;
	}
	logger.logMethod("INFO", "Setting up environment for CGI script: " + request.uri);
	env["REQUEST_METHOD"] = request.method;
	env["QUERY_STRING"] = request.uri.find('?') != std::string::npos ? request.uri.substr(request.uri.find('?') + 1) : "";
	env["CONTENT_TYPE"] = request.headers.count("content-type") ? request.headers["content-type"] : "";
	env["CONTENT_LENGTH"] = request.headers.count("content-length") ? request.headers["content-length"] : "0";
	env["SCRIPT_NAME"] = request.uri.substr(0, request.uri.find('?'));
	env["REMOTE_ADDR"] = "127.0.0.1";
	env["SERVER_NAME"] = "localhost";
	env["SERVER_PORT"] = std::to_string(port);

	// format each header with required cgi style and convert to lowecase
	for (const auto& header : request.headers)
	{
		std::string cgiHeader = "HTTP_" + header.first;
		std::replace(cgiHeader.begin(), cgiHeader.end(), '-', '_');
		std::transform(cgiHeader.begin(), cgiHeader.end(), cgiHeader.begin(), ::tolower);
		env[cgiHeader] = header.second;
	}

	std::vector<std::string> envp;
	for(const auto& [key, value] : env)
		envp.push_back(key + "=" + value); // converts it vector of strings for execve as it wont take a map
	executeCGI(request.uri, client_socket, envp);
}

void	redirectPipes(int* inputPipe, int* outputPipe, int flag)
{
	if (flag == 0)
	{
		dup2(outputPipe[1], STDOUT_FILENO); // redirect STDOUT to output pipe
		close(outputPipe[0]); // close read end
		close(outputPipe[1]); // close write end
	}
	else
	{
		dup2(inputPipe[0], STDIN_FILENO);
		close(inputPipe[1]);
		close(inputPipe[0]);
	}
}

void	HttpServer::executeCGI(const std::string& scriptPath, int client_socket, const std::vector<std::string>& envp)
{
	int inputPipe[2];
	int outputPipe[2];
	pipe(inputPipe);
	pipe(outputPipe);
	std::string correctScriptPath = removeLeading(scriptPath);
	std::string handler = findHandler(correctScriptPath, client_socket);

	if (handler.empty())
	{
		logger.logMethod("ERROR", "No CGI handler found for: " + correctScriptPath);
		sendErrorResponse(client_socket, 500, "Internal Server Error - No CGI handler found for: " + correctScriptPath);
		return ;
	}
	logger.logMethod("INFO", "Executing script woih handler: " + handler + " and script path: " + scriptPath);
	pid_t pid = fork();
	if (pid == 0)
	{
		// Child process
		redirectPipes(inputPipe, outputPipe, 0);
		redirectPipes(inputPipe, outputPipe, 1);

		char* args[] = {const_cast<char*>(handler.c_str()), const_cast<char*>(correctScriptPath.c_str()), nullptr};
		// convert into c-style string for execve
		std::vector<char*> envPtrs;
		for (const auto& envVariables : envp)
			envPtrs.push_back(const_cast<char*>(envVariables.c_str()));
		envPtrs.push_back(nullptr); // adds null-termination for string

		if (execve(handler.c_str(), args, envPtrs.data()) == -1) 
		{
			perror("execve failed");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid > 0)
	{
		// Parent process
		close(inputPipe[0]); // Close read end of input pipe
		close(outputPipe[1]); // Close write end of output pipe

		std::string cgiOutput;
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			cgiOutput.append(buffer);
		}
		close(outputPipe[0]);

		std::string body = parseCgiOutput(cgiOutput);

		logger.logMethod("INFO", "Output for CGI is: \n" + body);
		clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 200, "OK", body, false, clientInfoMap[client_socket].request.uri);
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			sendErrorResponse(client_socket, 500, "Internal Server Error - CGI script execution failed.");
			return ;
		}
		deregisterReadEvent(client_socket);
		registerWriteEvent(client_socket);
	}
	else
		sendErrorResponse(client_socket, 500, "Internal Server Error - Failed to fork script.");
}