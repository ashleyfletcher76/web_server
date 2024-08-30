#include "HttpServer.hpp"

std::string HttpServer::extractHeaderValue(const std::string &headers, const std::string &key)
{
	std::string searchKey = key + ": ";
	std::size_t start = headers.find(searchKey);
	if (start != std::string::npos)
	{
		start += searchKey.length();
		std::size_t end = headers.find("\r\n", start);
		if (end != std::string::npos)
			return headers.substr(start, end - start);
	}
	return "";
}

std::string HttpServer::extractFilename(const std::string &contentDisposition)
{
	std::string filenamePrefix = "filename=\"";
	std::size_t start = contentDisposition.find(filenamePrefix);
	if (start != std::string::npos)
	{
		start += filenamePrefix.length();
		std::size_t end = contentDisposition.find("\"", start);
		if (end != std::string::npos)
			return contentDisposition.substr(start, end - start);
	}
	return "";
}

void HttpServer::parseMultipartBody(const std::string &body, const std::string &boundary, HttpRequest &request)
{
	std::string delimiter = "--" + boundary;
	std::size_t start = 0;
	while ((start = body.find(delimiter, start)) != std::string::npos)
	{
		start += delimiter.length() + 2;
		std::size_t end = body.find(delimiter, start);
		if (end == std::string::npos)
			end = body.find("--", start);

		std::string part = body.substr(start, end - start);
		std::string headerData;
		std::string contentData;

		std::size_t headerEnd = part.find("\r\n\r\n");
		if (headerEnd != std::string::npos)
		{
			headerData = part.substr(0, headerEnd);
			contentData = part.substr(headerEnd + 4);
		}

		std::string contentDisposition = extractHeaderValue(headerData, "Content-Disposition");
		if (contentDisposition.find("filename=") != std::string::npos)
		{
			std::string filename = extractFilename(contentDisposition);
			request.files[filename] = contentData;
		}

		start = end + delimiter.length();
	}
}

std::string HttpServer::extractBoundary(const std::string &contentType)
{
	std::string boundaryPrefix = "boundary=";
	std::size_t start = contentType.find(boundaryPrefix);
	if (start != std::string::npos)
	{
		start += boundaryPrefix.length();
		std::size_t end = contentType.find(';', start);
		if (end == std::string::npos)
			end = contentType.length();
		return contentType.substr(start, end - start);
	}
	return "";
}

bool HttpServer::parseHttpRequestBody(std::istringstream &requestStream, HttpRequest &request, int client_socket)
{
	auto contentTypeIter = request.headers.find("content-type");
	if (contentTypeIter != request.headers.end() &&
		contentTypeIter->second.find("multipart/form-data") != std::string::npos)
	{
		std::string boundary = extractBoundary(contentTypeIter->second);
		if (boundary.empty())
			return false;

		request.body.assign((std::istreambuf_iterator<char>(requestStream)),
							std::istreambuf_iterator<char>());

		parseMultipartBody(request.body, boundary, request);
	}
	else if (request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		auto iter = request.headers.find("content-length");
		if (iter != request.headers.end())
		{
			int contentLength = std::stoi(iter->second);
			int maxSize = getMaxClientBodySize(client_socket);
			if (contentLength > maxSize)
			{
				return false;
			}
			std::getline(requestStream, request.body, '\0');
		}
	}
	else if (request.headers["content-type"] == "application/json")
	{
		auto iter = request.headers.find("content-length");
		if (iter != request.headers.end())
		{
			int contentLength = std::stoi(iter->second);
			int maxSize = getMaxClientBodySize(client_socket);
			if (contentLength > maxSize)
			{
				return false;
			}
			std::getline(requestStream, request.body, '\0');
		}
	}
	return true;
}