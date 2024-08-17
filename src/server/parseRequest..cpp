#include "HttpServer.hpp"

# define MAX_ALLOWED_BODY_SIZE 10000

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

bool HttpServer::parseHttpRequest(const std::string& requestStr, HttpRequest& request, int client_socket)
{
	std::istringstream requestStream(requestStr);
	std::string line;
	if (!std::getline(requestStream, line) || line.empty()) return (false);
	std::istringstream lineStream(line);
	(void)client_socket;
	// int server_fd = clientInfoMap[client_socket].server_fd;
	// servers[server_fd]->getserverInfo().client_max_body_size;
	// extract method, URI and version from request line
	if (!(lineStream >> request.method >> request.uri >> request.version)) return (false);
	if (!isValidMethod(request.method) || !isValidUri(request.uri)
		|| !isValidVersion(request.version)) return (false);

	// parse headers to start
	bool headersFinished = false;
	while (std::getline(requestStream, line))
	{
		if (line == "\r" || line.empty())
		{
			headersFinished = true;
			break ;
		}
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
	// parse data after headers(":")
	if (headersFinished && request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		auto iter = request.headers.find("content-length");
		if (iter != request.headers.end())
		{
			int contentLength = std::stoi(iter->second);
			if (contentLength > MAX_ALLOWED_BODY_SIZE)
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
