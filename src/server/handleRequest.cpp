#include "HttpServer.hpp"

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
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", fileContent);
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
	else
		sendErrorResponse(client_socket, 501, "Not Implemented");

	// set up write event for client response
	struct kevent change;
	EV_SET(&change, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	// validate fd before using
	if (fcntl(client_socket, F_GETFL) != -1)
	{
		if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
			log("ERROR", "Kevent registration failure for writing: " + std::string(strerror(errno)), client_socket);
		else
			log("INFO", "Succesfully registered kevent for socket: " + std::to_string(client_socket), NOSTATUS);
	}
	else
	{
		log ("ERROR", "Attempted to register kevent for invalid FD: " + std::to_string(client_socket), NOSTATUS);
		closeSocket(client_socket);
	}
}

// first iterator points to beginning of the file
// second used as end marker, correct syntax
