#include "HttpServer.hpp"

void	HttpServer::handleGetRequest(int client_socket)
{
	// checks path from URI provided in request
	std::string filePath = getFilePath(clientInfoMap[client_socket].request.uri);
	std::string uri = clientInfoMap[client_socket].request.uri;

	if (uri.find("/profile") == 0)
	{
		std::string profileID = uri.substr(9);
		userProfile profile;
		if (database.getUserProfile(profileID, profile))
		{
			std::string profileContent = generateProfilePage(profile);
			clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", profileContent, clientInfoMap[client_socket].shouldclose);
		}
		else
			sendErrorResponse(client_socket, 404, "Profile not found.");
	}
	else
	{
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

}
