#include "HttpServer.hpp"
#include <regex>

std::map<std::string, std::string> parseJSonBody(const std::string& body)
{
	std::map<std::string, std::string> data;
	std::regex jsonPattern(R"(\s*"\s*(\w+)\s*"\s*:\s*(\d+|".*?")\s*)");
	std::smatch matches;

	if (std::regex_search(body, matches, jsonPattern) && matches.size() > 2)
	{
		std::string key = matches[1];
		std::string value = matches[2];
		if (value.front() == '"' && value.back() == '"')
			value = value.substr(1, value.length() -2);
		data[key] = value;
	}
	return (data);
}

void	HttpServer::handleDeleteRequest(int client_socket, HttpRequest &request)
{
	auto formData = parseJSonBody(request.body);
	std::string responseBody;

	//std::cout << "Request body: " + formData << std::endl;
	if (formData.find("id") != formData.end())
	{
		std::string profileId = formData["id"];
		database.handleDeleteProfile(profileId);
		responseBody = "<html><body>User has been deleted successfully!</body></html>";
		clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, 
			"OK", responseBody, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
	}
	else if (formData.find("id") == formData.end())
	{
		logger.logMethod("ERROR", "No ID provided for deletion.");
		sendErrorResponse(client_socket, 400, "Bad Request: Missing ID");
		return ;
	}
	else
	{
		sendErrorResponse(client_socket, 500, "Failed to delete profile");
		return ;
	}
	deregisterReadEvent(client_socket);
	registerWriteEvent(client_socket);
}
