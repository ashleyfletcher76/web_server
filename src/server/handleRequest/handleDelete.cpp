#include "HttpServer.hpp"

// void	HttpServer::handleDeleteRequest(int client_socket)
// {
// 	HttpRequest& request = clientInfoMap[client_socket]->request;

// 	if (request.headers["path"].find("/deleteProfile") == std::string::npos)
// 		return ;
// 	std::string	uri = request.uri;
// 	std::size_t idPos = uri.find("id=");
// 	if (idPos == std::string::npos)
// 	{
// 		logger.logMethod("ERROR", "No ID found.");
// 		return ;
// 	}
// 	std::string idStr = uri.substr(idPos + 3);
// 	int	ID = std::stoi(idStr);
// 	if (database.deleteProfile(ID))
// 		// log and create a html for successful deletion
// 	else
// 		sendErrorResponse(client_socket, 500, "Failed to delete profile");
// 	return ;
// }
