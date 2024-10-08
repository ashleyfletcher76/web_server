#include "HttpServer.hpp"

std::map<std::string, std::string> mimeTypes = {
	{".html", "text/html"},
	{".css", "text/css"},
	{".js", "application/javascript"}
};

std::string	getMimeType(const std::string& filePath)
{
	std::size_t	dotPos = filePath.rfind('.');
	if (dotPos != std::string::npos)
	{
		std::string extension = filePath.substr(dotPos);
		if (mimeTypes.count(extension))
			return (mimeTypes[extension]);
		else
			return ("text/html");

	}
	return ("text/html");
}

std::string HttpServer::formatHttpResponse(const std::string& httpVersion, int status_code, const std::string &reasonPhrase,
										   const std::string &body, int keepAlive, const std::string& filePath)
{
	std::ostringstream response;
	std::string httpVersion1 = httpVersion;
	std::string	mimeType = getMimeType(filePath);
	if (httpVersion.empty())
	{
		httpVersion1 = "HTTP/1.1";
	}
	response << httpVersion1 << " " << std::to_string(status_code) << " " << reasonPhrase << "\r\n";
	response << "Content-Length: " << std::to_string(body.size()) << "\r\n";
	response << "Content-Type: " + mimeType + "; charset=UTF-8\r\n";
	if (keepAlive)
		response << "Connection: close\r\n";
	else
		response << "Connection: keep-alive\r\n";
	response << "\r\n";
	response << body;
	return (response.str());
}

std::string createHttpDownloadResponse(const std::string &version, int statusCode, const std::string &statusMessage,
							   const std::string &body, const std::string &headers)
{
	std::ostringstream responseStream;
	responseStream << version << " " << statusCode << " " << statusMessage << "\r\n"
				   << headers
				   << "Connection: close\r\n" // Connection will be closed after the response
				   << "\r\n"				  // End of headers
				   << body;					  // Body of the response
	return responseStream.str();
}