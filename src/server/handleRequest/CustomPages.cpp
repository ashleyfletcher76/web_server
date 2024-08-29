#include "HttpServer.hpp"

bool HttpServer::findProfileByID(const std::string &uri, int client_socket)
{
	std::size_t queryStart = uri.find('?');
	if (queryStart == std::string::npos)
		return (false);
	std::string queryString = uri.substr(queryStart + 1);
	// split query string based on '&' and '=' to get key values
	std::unordered_map<std::string, std::string> queryParams;
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
		return (false);
	std::string profileID = queryParams["id"];
	userProfile profile;
	if (!database.getUserProfile(profileID, profile))
	{
		return (false);
	}
	std::string profileContent = generateProfilePage(profile);
	clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", profileContent, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
	return (true);
}

void HttpServer::uploadsPage(int client_socket)
{
	std::ostringstream responseBody;
	responseBody << "<html><body><h1>Uploaded Files</h1><ul>";

	for (const auto &entry : std::filesystem::directory_iterator("./uploads/"))
	{
		std::string filename = entry.path().filename().string();
		responseBody << "<li><a href=\"/uploads/" << filename << "\">" << filename << "</a></li>";
	}
	responseBody << "</ul></body></html>";
	clientResponse[client_socket] = formatHttpResponse(
		clientInfoMap[client_socket].request.version, 200, "OK", responseBody.str(), clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);

}

void HttpServer::serveFile(int client_socket, const std::string &uri)
{
	std::string filepath = "." + uri;

	std::ifstream file(filepath, std::ios::binary);
	if (file.is_open())
	{
		std::ostringstream fileData;
		fileData << file.rdbuf();
		file.close();

		std::string contentType = "application/octet-stream";
		if (ends_with(uri, ".html"))
			contentType = "text/html";
		else if (ends_with(uri, ".jpg") || ends_with(uri, ".jpeg"))
			contentType = "image/jpeg";
		else if (ends_with(uri, ".png"))
			contentType = "image/png";
		else if (ends_with(uri, ".pdf"))
			contentType = "application/pdf";

		std::ostringstream responseHeaders;
		responseHeaders << "Content-Type: " << contentType << "\r\n"
						<< "Content-Disposition: attachment; filename=\""
						<< filepath.substr(filepath.find_last_of("/\\") + 1) << "\"\r\n"
						<< "Content-Length: " << fileData.str().size() << "\r\n";

		clientResponse[client_socket] = createHttpDownloadResponse(
			clientInfoMap[client_socket].request.version, 200, "OK", fileData.str(), responseHeaders.str());

	}
	else
	{
		sendErrorResponse(client_socket, 404, "File not found!");
		return;
	}
}