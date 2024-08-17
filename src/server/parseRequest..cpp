#include "HttpServer.hpp"

bool	isValidVersion(const std::string& version)
{
	return (version == "HTTP/1.0" || version == "HTTP/1.1"); // support only these versions
}

bool	isValidUri(const std::string& uri)
{
	return (uri.find("..") == std::string::npos); // prevent directory traversal
}

bool	isValidMethod(const std::string& method)
{
	std::set<std::string> validMethods;
	validMethods.insert("GET");
	validMethods.insert("POST");
	validMethods.insert("DELETE");
	return (validMethods.find(method) != validMethods.end());
}

bool	isValidHeader(const std::string& name, const std::string& value)
{
	return (name.find('\n') == std::string::npos && value.find('\n') == std::string::npos);
}

void	normaliseHeader(std::string& header)
{
	for(std::size_t i = 0; i <header.size(); i++)
		header[i] = std::tolower(header[i]);
}

bool	HttpServer::parseHttpRequestHeaders(std::istringstream& requestStream, HttpRequest& request)
{
	std::string	line;
	if (!std::getline(requestStream, line) || line.empty())
		return (false);
	std::istringstream	lineStream(line);
	if (!(lineStream >> request.method >> request.uri >> request.version))
		return (false);
	if (!isValidMethod(request.method) || !isValidUri(request.uri)
		|| !isValidVersion(request.version))
		return (false);
	// parse headers to start
	while (std::getline(requestStream, line) && line != "\r" && !line.empty())
	{
		std::size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue ;
		std::string headerName = line.substr(0, colonPos);
		std::string headerValue = line.substr(colonPos + 2);
		trim(headerName);
		trim(headerValue);
		normaliseHeader(headerName);
		request.headers[headerName] = headerValue;
	}
	return (true);
}

bool	HttpServer::parseHttpRequestBody(std::istringstream& requestStream, HttpRequest& request, int client_socket)
{
	if (request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		auto iter = request.headers.find("content-length");
		if (iter != request.headers.end())
		{
			int contentLength = std::stoi(iter->second);
			int	maxSize = getMaxClientBodySize(client_socket);
			if (contentLength > maxSize)
			{
				logger.logMethod("ERROR", "Content body is too large.");
				// add a new html here
				return (false);
			}
			std::getline(requestStream, request.body); // get the content data for POST
		}
	}
	return (true);
}

bool HttpServer::parseHttpRequest(const std::string& requestStr, HttpRequest& request, int client_socket)
{
	std::istringstream requestStream(requestStr);

	if (!parseHttpRequestHeaders(requestStream, request))
		return (false);

	if (!parseHttpRequestBody(requestStream, request, client_socket))
	{
		sendErrorResponse(client_socket, 413, "Payload too large.");
		return (false);
	}
	return (true);
}
