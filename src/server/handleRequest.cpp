#include "HttpServer.hpp"

void	HttpServer::handlePostRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;
	std::string responseBody;

	std::cout << "Here begin POST" << std::endl;
	if (request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		std::cout << "Here begin header getline" << std::endl;
		std::map<std::string, std::string> formData;
		std::istringstream bodyStream(request.body);
		std::string pair;
		while (std::getline(bodyStream, pair, '&'))
		{
			size_t equals = pair.find('=');
			if (equals != std::string::npos)
			{
				std::string key = pair.substr(0, equals);
				std::string value = pair.substr(equals + 1);
				formData[key] = value;
			}
		}
		// log data into a file
		std::ofstream logFile("userLog.txt", std::ios::app);
		if (logFile.is_open())
		{
			logFile << "New POST request: \n";
			for (std::map<std::string, std::string>::const_iterator iter = formData.begin(); iter != formData.end(); iter++)
				logFile << iter->first << ": " << iter->second << std::endl;
			logFile << "--------------\n";
			logger.logMethod("INFO", "New log file updated");
			logFile.close();
			std::cout << "Here inside logFile success" << std::endl;
		}
		else
		{
			std::cout << "Here inside logFile fail" << std::endl;
			logger.logMethod("ERROR", "Could not open log file.");
		}
		// create response from form data
		responseBody = "<html><body>POST data recieved:<br>"; // change for an actual html file
		for (std::map<std::string, std::string>::const_iterator iter = formData.begin(); iter != formData.end(); iter++)
			responseBody += iter->first + ": " + iter->second + "<br>";
		responseBody += "</body></html>"; // change for an actual html file
	}
	else if (request.headers["Content-Type"] == "application/json")
		responseBody = "<html><body>JSON data recieved</body></html>";
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
	writeResponse(client_socket);
}

void	HttpServer::handleGetRequest(int client_socket)
{
	// checks path from URI provided in request
	std::string filePath = getFilePath(clientInfoMap[client_socket].request.uri);

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		sendErrorResponse(client_socket, 404, "Not Found");
		return ;
	}
	// read the whole content of the file
	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // istreambuf_iterator is efficient for unformated data reading(raw bytes)
	file.close();
	// set response in the clients info
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", fileContent, clientInfoMap[client_socket].shouldclose);
}

void	HttpServer::handleRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;

	// decide to keep connection open based on HTTP response
	if (request.headers.find("connection") != request.headers.end())
	{
		std::string connectionValue = request.headers["connection"];
		trim(connectionValue);
		std::transform(connectionValue.begin(), connectionValue.end(), connectionValue.begin(), ::tolower);
		if (connectionValue == "keep-alive")
			clientInfoMap[client_socket].shouldclose = false;
		else
			clientInfoMap[client_socket].shouldclose = true;
	}
	else
		clientInfoMap[client_socket].shouldclose = true;
	if (request.method == "GET")
		handleGetRequest(client_socket);
	else if (request.method == "POST")
		handlePostRequest(client_socket);
	else
		sendErrorResponse(client_socket, 501, "Not Implemented");

	// set up write event for client response
	struct kevent change;
	EV_SET(&change, static_cast<uintptr_t>(client_socket), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	// validate fd before using
	if (fcntl(client_socket, F_GETFL) != -1)
	{
		if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
			logger.logMethod("ERROR", "Kevent registration failure for writing: " + std::string(strerror(errno)));
		else
			logger.logMethod("INFO", "Succesfully registered kevent for socket: " + std::to_string(client_socket));
	}
	else
	{
		logger.logMethod ("ERROR", "Attempted to register kevent for invalid FD: " + std::to_string(client_socket));
		closeSocket(client_socket);
	}
}

// first iterator points to beginning of the file
// second used as end marker, correct syntax
