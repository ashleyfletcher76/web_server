#include "HttpServer.hpp"

bool	HttpServer::findProfileByID(const std::string& uri, int client_socket)
{
	std::size_t queryStart = uri.find('?');
	if (queryStart == std::string::npos)
	{
		std::cout << "Inside queryStart fail" << std::endl;
		return (false);
	}
	std::string queryString = uri.substr(queryStart + 1);
	// split query string based on '&' and '=' to get key values
	std::map<std::string, std::string> queryParams;
	std::istringstream queryStream(queryString);
	std::string pair;
	while (std::getline(queryStream, pair, '&'))
	{
		std::size_t eqPos = pair.find('=');
		if (eqPos != std::string::npos)
		{
			std::string key = pair.substr(0, eqPos);
			std::string value = pair.substr(eqPos + 1);
			queryParams[key] = value;
		}
	}
	// check if 'id' exists
	if (queryParams.find("id") == queryParams.end())
	{
		std::cout << "Inside find if fail" << std::endl;
		return (false);
	}
	std::string profileID = queryParams["id"];
	userProfile profile;
	if (!database.getUserProfile(profileID, profile))
	{
		std::cout << "Inside getUserProfile fail" << std::endl;
		return (false);
	}
	std::string profileContent = generateProfilePage(profile);
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", profileContent, clientInfoMap[client_socket].shouldclose);
	return (true);
}

void	HttpServer::handleGetRequest(int client_socket)
{
	// checks path from URI provided in request
	std::string filePath = getFilePath(clientInfoMap[client_socket].request.uri);
	std::string uri = clientInfoMap[client_socket].request.uri;

	if (uri == "/allProfiles.html")
		generateAllProfilesPage(client_socket);
	else if (uri.find("/profile") == 0)
	{
		if (!findProfileByID(uri, client_socket))
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
