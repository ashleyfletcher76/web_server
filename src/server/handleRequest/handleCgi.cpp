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

	env["REQUEST_METHOD"] = request.method;
	env["QUERY_STRING"] = request.uri.find('?') != std::string::npos ? request.uri.substr(request.uri.find('?') + 1) : "";
	env["CONTENT_TYPE"] = request.headers.count("content-type") ? request.headers["content-type"] : "";
	env["CONTENT_LENGTH"] = request.headers.count("content-length") ? request.headers["content-length"] : "0";
	env["SCRIPT_NAME"] = request.uri.substr(0, request.uri.find('?'));
	env["REMOTE_ADDR"] = "127.0.0.1";
	env["SERVER_NAME"] = "localhost";
	env["SERVER_PORT"] = std::to_string(port);


	for (const auto& header : request.headers)
	{
		std::string cgiHeader = "HTTP_" + header.first;
		std::replace(cgiHeader.begin(), cgiHeader.end(), '-', '_');
		std::transform(cgiHeader.begin(), cgiHeader.end(), cgiHeader.begin(), ::tolower);
		env[cgiHeader] = header.second;
	}

	std::vector<std::string> envp;
	for(const auto& [key, value] : env)
		envp.push_back(key + "=" + value);
	std::string scriptPath = removeLeading(request.uri);
	// std::cout << scriptPath << std::endl;
	executeCGI(scriptPath, client_socket, envp);
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

std::string	parseCgiOutput(std::string cgiOutput)
{
	// Parsing CGI output
	std::string body;
	std::string header;
	bool headerFinished = false;
	std::istringstream iss(cgiOutput);
	std::string line;
	while (std::getline(iss, line))
	{
		if (!headerFinished)
		{
			if (line.empty() || line == "\r")
				headerFinished = true;
			else
				header += line + "\n";
		}
		else
			body += line + "\n";
	}
	return (body);
}

void	HttpServer::executeCGI(const std::string& scriptPath, int client_socket, const std::vector<std::string>& envp)
{
	int inputPipe[2];
	int outputPipe[2];
	pipe(inputPipe);
	pipe(outputPipe);

	pid_t pid = fork();
	if (pid == 0)
	{
		// Child process
		redirectPipes(inputPipe, outputPipe, 0);
		redirectPipes(inputPipe, outputPipe, 1);

		std::vector<char*> envPtrs;
		for (const auto& e : envp)
			envPtrs.push_back(const_cast<char*>(e.c_str()));
		envPtrs.push_back(nullptr);

		if (execve(scriptPath.c_str(), nullptr, envPtrs.data()) == -1) 
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

		logger.logMethod("INFO", "Output for CGI is: " + body);
		clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 200, "OK", body, false);

		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 500, "Internal Server Error", 
				"CGI script execution failed", true);
	}
	else
	{
		perror("Failed to fork");
		clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 500, "Internal Server Error", 
			"Failed to fork process", true);
	}
}