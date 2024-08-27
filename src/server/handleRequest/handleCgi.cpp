#include "HttpServer.hpp"

std::string removeLeading(const std::string& path)
{
    if (!path.empty() && path[0] == '/')
        return (path.substr(1));
    return (path);
}

void    HttpServer::setupCgiEnvironment(int client_socket)
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

    //std::cout << request.method << std::endl;

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

void    redirectPipes(int* inputPipe, int* outputPipe, int flag)
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

void HttpServer::executeCGI(const std::string& scriptPath, int client_socket, const std::vector<std::string>& envp)
{   
    int inputPipe[2];
    int outputPipe[2];
    pipe(inputPipe);
    pipe(outputPipe);

    pid_t pid = fork();
    if (pid == 0)
    {
        redirectPipes(inputPipe, outputPipe, 0);
        redirectPipes(inputPipe, outputPipe, 1);

        std::vector<char*> envPtrs;
        std::vector<std::string> envStrings(envp.begin(), envp.end());
        std::transform(envStrings.begin(), envStrings.end(), std::back_inserter(envPtrs), [](const std::string& str) {
            return const_cast<char*>(str.c_str()); // Get the raw pointers
        });
        envPtrs.push_back(nullptr); // Terminate array

        if (execve(scriptPath.c_str(), nullptr, envPtrs.data()) == -1) 
        {
            perror("execve failed");
            logger.logMethod("ERROR", "Execve failed");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        close(outputPipe[1]);
        close(inputPipe[0]);

        std::string cgiOutput, line;
        std::stringstream ss;
        bool headerFinished = false;
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            if (bytesRead <= 0)
                break ;
            buffer[bytesRead] = '\0';
            ss << buffer;
            while (std::getline(ss, line, '\n'))
            {
                if (!headerFinished)
                {
                    if (line == "\r" || line.empty())
                        headerFinished = true;
                }
                else
                    cgiOutput += line + "\n";
            }
        }
        close(outputPipe[0]);
        logger.logMethod("INFO", "--------" + cgiOutput);
        clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 200, "OK", cgiOutput, true);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            clientResponse[client_socket] = formatHttpResponse("HTTP/1.1", 500, "Internal Server Error", "CGI script execution failed", true);
    }
    else
        perror("Failed to fork");
}
