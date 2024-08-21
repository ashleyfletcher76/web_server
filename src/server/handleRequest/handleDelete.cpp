#include "HttpServer.hpp"

void	HttpServer::handleDeleteRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;
	auto formData = parseFormData(request.body);
	std::string responseBody;

	auto iter = formData.find("id");
	if (iter == formData.end())
	{
		logger.logMethod("ERROR", "No ID provided for deletion.");
		sendErrorResponse(client_socket, 400, "Bad Request: Missing ID");
		return ;
	}
	if (database.handleDeleteProfile(iter->second))
	{
		logger.logMethod("INFO", "Deletion completed.");
		responseBody = "<html><body>User has been deleted successfully!</body></html>";
		clientInfoMap[client_socket].response = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
	}
	else
		sendErrorResponse(client_socket, 500, "Failed to delete profile");
}
