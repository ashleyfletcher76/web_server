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

void	HttpServer::trim(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), isNotSpace()));
	str.erase(std::find_if(str.rbegin(), str.rend(), isNotSpace()).base(), str.end());
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

bool HttpServer::parseHttpRequest(const std::string& requestStr, HttpRequest& request)
{
	std::istringstream requestStream(requestStr);
	std::string line;
	if (!std::getline(requestStream, line) || line.empty())
		return (false);
	std::istringstream lineStream(line);
	// extract method, URI and version from request line
	if (!(lineStream >> request.method >> request.uri >> request.version))
		return (false);
	if (!isValidMethod(request.method) || !isValidUri(request.uri)
		|| !isValidVersion(request.version))
		return (false);

	while (std::getline(requestStream, line) && line != "\r")
	{
		std::string::size_type colonPos = line.find(':');
		if (colonPos == std::string::npos)
			return (false);
		std::string headerName = line.substr(0, colonPos);
		std::string headerValue = line.substr(colonPos + 2);
		trim(headerName);
		trim(headerValue);
		if (!isValidHeader(headerName, headerValue))
			return (false);
		normaliseHeader(headerName);
		request.headers[headerName] = headerValue;
		//std::cout << headerName << " = " << headerValue << std::endl;
	}
	return (true);
}
