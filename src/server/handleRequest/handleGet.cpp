#include "HttpServer.hpp"


void HttpServer::handleGetRequest(int client_socket)
{
	int server_fd = clientInfoMap[client_socket].server_fd;
	std::string filePath = getFilePath(server_fd, clientInfoMap[client_socket].request.uri);
	std::string uri = clientInfoMap[client_socket].request.uri;

	if (uri == "/allProfiles.html")
	{
		generateAllProfilesPage(client_socket);
	}
	else if (uri.find("/cgi-bin/") == 0)
	{
		setupCgiEnvironment(client_socket);
	}
	else if (uri.find("/profile") == 0)
	{
		if (!findProfileByID(uri, client_socket))
		{
			sendErrorResponse(client_socket, 404, "Not Found");
		}
	}
	else if (uri == "/download")
	{
		uploadsPage(client_socket);
	}
	else if (uri.rfind("/uploads/", 0) == 0)
	{
		serveFile(client_socket, uri);
	}
	else
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			sendErrorResponse(client_socket, 404, "Not Found");
			return;
		}
		std::string fileContent((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
		file.close();
		clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", 
			fileContent, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
		registerWriteEvent(client_socket);
	}
}
