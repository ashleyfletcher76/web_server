#include "HttpServer.hpp"

void	replacePlaceholders(std::string& content, const std::string& placeholder, const std::string& value)
{
	std::size_t pos = content.find(placeholder);
	while (pos != std::string::npos)
	{
		content.replace(pos, placeholder.length(), value);
		pos = content.find(placeholder, pos + value.length());
	}
}

void	HttpServer::generateAllProfilesPage(int client_socket)
{
	std::vector<userProfile> profiles;
	if (database.fetchAllProfiles(profiles))
	{
		std::string pageContent = readFileContent("www/allProfiles.html");
		std::string profileLinks;
		for (const auto& profile : profiles)
		{
			profileLinks += "<tr>"  // start of table row
				"<td>" + std::to_string(profile.id) + "</td>"  // ID column
				"<td><a href='/profile?id=" + std::to_string(profile.id) + "'>" + profile.name + "</a></td>"  // name column
				"<td><button onclick='deleteProfile(" + std::to_string(profile.id) + ")'>Delete</button></td>"
				"</tr>";  // end of table row

		}
		replacePlaceholders(pageContent, "<!-- Placeholder -->", profileLinks);
		clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", 
			pageContent, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
		registerWriteEvent(client_socket);
	}
	else
	{
		sendErrorResponse(client_socket, 500, "Internal Server Error");
	}
}


std::string	HttpServer::generateProfilePage(const userProfile& profile)
{
	std::string filePath = "www/profile.html";
	std::string content = readFileContent(filePath);

	replacePlaceholders(content, "{{name}}", profile.name);
	replacePlaceholders(content, "{{email}}", profile.email);
	replacePlaceholders(content, "{{phoneNum}}", profile.phoneNum);
	replacePlaceholders(content, "{{description}}", profile.description);

	return (content);
}

