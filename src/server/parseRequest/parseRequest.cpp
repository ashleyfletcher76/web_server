#include "HttpServer.hpp"

bool isValidVersion(std::string_view version)
{
	return (version == "HTTP/1.0" || version == "HTTP/1.1"); // support only these versions
}

bool isValidUri(std::string_view uri)
{
	return (uri.find("..") == std::string::npos); // prevent directory traversal
}

bool isValidMethod(HttpRequest &request, const std::string &method)
{
	static const std::set<std::string> validMethods = {"GET", "POST", "DELETE"};
	request.validmethod = validMethods.find(method) != validMethods.end();
	return (request.validmethod);
}

bool isValidHeader(std::string_view name, std::string_view value)
{
	return (name.find('\n') == std::string_view::npos && value.find('\n') == std::string_view::npos);
}

void normaliseHeader(std::string &header)
{
	std::transform(header.begin(), header.end(), header.begin(), ::tolower);
}

bool isCgiRequest(const std::string &uri)
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
		logger.logMethod("ERROR", "Fail is stream inside request headers");
		return (false);
	}
	if (!isValidMethod(request, request.method) || !isValidUri(request.uri) || !isValidVersion(request.version))
	{
		logger.logMethod("ERROR", "Fail is in valid inside request headers");
		return (false);
	}
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
			logger.logMethod("ERROR", "Fail is in getline inside request headers");
			return false;
		}
		request.headers[headerName] = headerValue;
	}
	if (request.headers.find("host") == request.headers.end())
	{
		logger.logMethod("ERROR", "Fail is in getline inside request headers in host");
		return false;
	}
	return (true);
}

bool HttpServer::parseHttpRequest(const std::string &requestStr, HttpRequest &request, int client_socket)
{

	std::istringstream requestStream(requestStr);
	if (!parseHttpRequestHeaders(requestStream, request))
	{
		if (!request.validmethod)
			sendErrorResponse(client_socket, 405, "Method Not Allowed");
		else
		{
			sendErrorResponse(client_socket, 400, "Bad request");
		}
		return (false);
	}
	if (!parseHttpRequestBody(requestStream, request, client_socket))
	{
		sendErrorResponse(client_socket, 413, "Payload too large.");
		return (false);
	}
	return (true);
}
