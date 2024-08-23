#include "HttpServer.hpp"

std::string	urlDecode(const std::string& str)
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

std::map<std::string, std::string>	HttpServer::parseFormData(const std::string& body)
{
	std::map<std::string, std::string> data;
	std::istringstream bodyStream(body);
	std::string pair;
	while (std::getline(bodyStream, pair, '&'))
	{
		size_t equals = pair.find('=');
		if (equals != std::string::npos)
		{
			std::string key = pair.substr(0, equals);
			std::string value = urlDecode(pair.substr(equals + 1));
			data[key] = value;
		}
	}
	return (data);
}

void	HttpServer::handlePostRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;
	std::string responseBody;

	if (request.headers["content-type"] != "application/x-www-form-urlencoded")
	{
		sendErrorResponse(client_socket, 415, "Unsupported medid type");
		return ;
	}
	if (request.uri.find("/deleteProfile") != std::string::npos)
	{
		handleDeleteRequest(client_socket);
		return ;
	}
	std::map<std::string, std::string> formData = parseFormData(request.body);
	if (!formData.empty())
	{
		// use formData to insert into database
		if (database.addUser(formData["name"], formData["email"],
			formData["phone"], formData["description"]))
		{
			responseBody = "<html><body>New user added successfully!</body></html>";
			clientInfoMap[client_socket].response = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
		}
		else
		{
			sendErrorResponse(client_socket, 500, "Internal Server Error");
			return ;
		}
	}
	else
	{
		responseBody = "<html><body>Empty Post request!</body></html>";
		clientInfoMap[client_socket].response = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
	}
	deregisterReadEvent(client_socket);
	registerWriteEvent(client_socket);
}
