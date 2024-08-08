#include "HttpServer.hpp"

void	HttpServer::handleGetRequest(int client_socket)
{
	std::string filePath = getFilePath(clientInfoMap[client_socket].request.uri);

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		sendErrorResponse(client_socket, 404, "Not Found");
		return ;
	}
	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", fileContent);
}

void	HttpServer::handleRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;

	// decide to keep connection open based on HTTP response
	clientInfoMap[client_socket].shouldclose = (request.headers["Connection"] != "keep-alive");

	if (request.method == "GET")
		handleGetRequest(client_socket);
	else
		sendErrorResponse(client_socket, 501, "Not Implemented");
	struct kevent change;
	EV_SET(&change, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
		log("ERROR", "Kevent registration failure for sriting: " + std::string(strerror(errno)), client_socket);
}
