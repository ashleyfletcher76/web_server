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

