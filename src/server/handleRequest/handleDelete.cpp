#include "HttpServer.hpp"

// void	HttpServer::handleDeleteRequest(int client_socket)
// {
// 	std::string	uri = clientInfoMap[client_socket]->request.uri;
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
// }
