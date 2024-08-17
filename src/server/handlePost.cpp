#include "HttpServer.hpp"

std::string	HttpServer::urlDecode(const std::string& str)
{
	std::string decoded;
	char	hex[3] = {0};

	for(std::size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '%')
		{
			if (i + 2 < str.length())
			{
				hex[0] = str[i + 1];
				hex[1] = str[i + 2];
				decoded += static_cast<char>(std::strtol(hex, nullptr, 16));
				i += 2;
			}
		}
		else if (str[i] == '+')
			decoded += ' ';
		else
			decoded += str[i];
	}
	return (decoded);
}

void	HttpServer::handlePostRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;
	std::string responseBody;

	if (request.headers["content-type"] != "application/x-www-form-urlencoded")
	{
		sendErrorResponse(client_socket, 415, "Unsupported medid type");
		std::cout << "Here" << std::endl;
		return ;
	}
	std::istringstream bodyStream(request.body);
	std::string pair;
	while (std::getline(bodyStream, pair, '&'))
	{
		size_t equals = pair.find('=');
		if (equals != std::string::npos)
		{
			std::string key = pair.substr(0, equals);
			std::string value = urlDecode(pair.substr(equals + 1));
			if (key == "name") request.userProfile.name = value;
			else if (key == "email") request.userProfile.email = value;
			else if (key == "phone") request.userProfile.phoneNum = value;
			else if (key == "description") request.userProfile.description = value;
		}
	}
	// use formData to insert into database
	if (!database.addUser(request.userProfile.name, request.userProfile.email,
		request.userProfile.phoneNum, request.userProfile.description))
	{
		sendErrorResponse(client_socket, 500, "Internal Server Error");
		return ;
	}
	responseBody = "<html><body>New user added successfully!</body></html>";
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
	writeResponse(client_socket);
}
