#include "HttpServer.hpp"

std::string HttpServer::formatHttpResponse(int status_code, const std::string& reasonPhrase,
	const std::string& body)
{
	std::ostringstream response;

	// constructs proper format for HTTP response
	response << "HTTP/1.1 " << status_code << " " << reasonPhrase << "\r\n";
	response << "Content-Length: " << body.size() << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << body;
	return (response.str());
}

bool HttpServer::parseHttpRequest(const std::string& requestStr, HttpRequest& request)
{
	std::istringstream requestStream(requestStr);
	std::string line;
	if (!std::getline(requestStream, line))
		return (false);
	std::istringstream lineStream(line);
	// extract method, URI and version from request line
	lineStream >> request.method;
	lineStream >> request.uri;
	lineStream >> request.version;

	while (std::getline(requestStream, line) && line != "\r")
	{
		std::istringstream headerStream(line);
		std::string headerName;
		// parse header name
		std::getline(headerStream,headerName, ':');
		std::string headerValue;
		// parse header value
		std::getline(headerStream, headerValue);
		request.headers[headerName] = headerValue;
	}
	return (true);
}
