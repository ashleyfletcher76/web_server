#include "HttpServer.hpp"

bool isValidVersion(std::string_view version)
{
	return (version == "HTTP/1.0" || version == "HTTP/1.1"); // support only these versions
}

bool isValidUri(std::string_view uri)
{
	return (uri.find("..") == std::string::npos); // prevent directory traversal
}

bool isValidMethod(const std::string &method)
{
	static const std::set<std::string> validMethods = {"GET", "POST"};
	return (validMethods.find(method) != validMethods.end());
}

bool isValidHeader(std::string_view name, std::string_view value)
{
	return (name.find('\n') == std::string_view::npos && value.find('\n') == std::string_view::npos);
}

void normaliseHeader(std::string &header)
{
	std::transform(header.begin(), header.end(), header.begin(), ::tolower);
}

bool	isCgiRequest(const std::string& uri)
{
	return (uri.find(".cgi") != std::string::npos || uri.find("/cgi-bin/") != std::string::npos);
}

bool HttpServer::parseHttpRequestHeaders(std::istringstream &requestStream, HttpRequest &request)
{
	std::string line;
	if (!std::getline(requestStream, line) || line.empty())
		return (false);
	std::istringstream lineStream(line);
	if (!(lineStream >> request.method >> request.uri >> request.version))
	{
		return (false);
	}
	if (!isValidMethod(request.method) || !isValidUri(request.uri) || !isValidVersion(request.version))
		return (false);
	while (std::getline(requestStream, line) && line != "\r" && !line.empty())
	{
		auto colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue;
		std::string headerName = line.substr(0, colonPos);
		std::string headerValue = line.substr(colonPos + 2);
		trim(headerName);
		trim(headerValue);
		normaliseHeader(headerName);
		if (!isValidHeader(headerName, headerValue))
		{
			return false;
		}
		request.headers[headerName] = headerValue;
	}
	if (request.headers.find("host") == request.headers.end())
	{
		return false;
	}
	return (true);
}

bool HttpServer::parseHttpRequestBody(std::istringstream &requestStream, HttpRequest &request, int client_socket)
{
	if (request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		auto iter = request.headers.find("content-length");
		if (iter != request.headers.end())
		{
			int contentLength = std::stoi(iter->second);
			int maxSize = getMaxClientBodySize(client_socket);
			if (contentLength > maxSize)
			{
				return (false);
			}
			std::getline(requestStream, request.body, '\0');
		}
	}
	return (true);
}


bool HttpServer::parseHttpRequest(const std::string &requestStr, HttpRequest &request, int client_socket)
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
