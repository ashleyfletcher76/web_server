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
	// if (database.getAllProfiles(profiles))
	// {
		std::string pageContent = readFileContent("html/allProfiles.html");
		std::string profileLinks;
		for (const auto& profile : profiles)
			profileLinks += "<li><a href='/profile?id=" + std::to_string(profile.id) + "'>" + profile.name + "</a></li>";
		replacePlaceholders(pageContent, "<!-- Profile links will be dynamically inserted here -->", profileLinks);
		clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", pageContent, clientInfoMap[client_socket].shouldclose);
	// }
	// else
		//sendErrorResponse(client_socket, 500, "Internal Server Error");
}

std::string	HttpServer::generateProfilePage(const userProfile& profile)
{
	std::string filePath = "html/profile.html";
	std::string content = readFileContent(filePath);

	replacePlaceholders(content, "{{name}}", profile.name);
	replacePlaceholders(content, "{{email}}", profile.email);
	replacePlaceholders(content, "{{phoneNum}}", profile.phoneNum);
	replacePlaceholders(content, "{{description}}", profile.description);

	return (content);
}
