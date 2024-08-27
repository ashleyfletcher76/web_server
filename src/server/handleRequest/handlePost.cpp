#include "HttpServer.hpp"

std::string urlDecode(const std::string &str)
{
	std::string decoded;
	char hex[3] = {0};

	for (std::size_t i = 0; i < str.length(); i++)
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

std::map<std::string, std::string> HttpServer::parseFormData(const std::string &body)
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

void HttpServer::handleSubmitForm(int client_socket, HttpRequest &request)
{
	std::string responseBody;
	std::map<std::string, std::string> formData = parseFormData(request.body);
	if (!formData.empty())
	{
		if (database.addUser(formData["name"], formData["email"],
							 formData["phone"], formData["description"]))
		{
			responseBody = "<html><body>New user added successfully!</body></html>";
			clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
		}
		else
		{
			sendErrorResponse(client_socket, 500, "Internal Server Error");
			return;
		}
	}
	deregisterReadEvent(client_socket);
	registerWriteEvent(client_socket);
}

void HttpServer::handlePostRequest(int client_socket)
{
	HttpRequest &request = clientInfoMap[client_socket].request;
	std::string contentType = request.headers["content-type"];
	std::string responseBody;

	if (contentType != "application/x-www-form-urlencoded" && (contentType.find("multipart/form-data") == std::string::npos))
	{
		sendErrorResponse(client_socket, 415, "Unsupported medid type");
		return;
	}
	if (request.uri.find("/deleteProfile") != std::string::npos)
	{
		handleDeleteRequest(client_socket, request);
		return;
	}
	if (request.uri.find("/submit") != std::string::npos)
	{
		handleSubmitForm(client_socket, request);
		return;
	}
	if (request.uri.find("/upload") != std::string::npos)
	{
		handleUpload(client_socket, request);
		return;
	}
	else
	{
		responseBody = "<html><body>Empty Post request!</body></html>";
		clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
		deregisterReadEvent(client_socket);
		registerWriteEvent(client_socket);
	}
}
