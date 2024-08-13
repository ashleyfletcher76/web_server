#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "includes.hpp"

struct isNotSpace
{
	bool operator()(unsigned char ch) const {
		return (!(std::isspace(ch) || ch == '\r' || ch == '\n'));
	}
};

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string body;
	std::string version;
	std::map<std::string, std::string> headers;
};

struct HttpResponse
{
	int statusCode;
	std::string reason;
	std::string body;
	std::map<std::string, std::string> headers;
};

struct ClientInfo
{
	HttpRequest	request;
	std::string	response;
	bool		shouldclose;

	ClientInfo() : shouldclose(false) {}
};

struct routeConfig
{
	std::string path;
	std::vector<std::string> allowedMethods;
	std::string directoryListing;
	std::string handler;
};

struct cgiConfig
{
	std::string extension;
	std::string handler;
};

struct serverInfo
{
	int				listen;
	std::string		host;
	std::string		server_name;
	std::string		document_root;
	std::string		default_file;
	std::string		client_max_body_size;
	std::string		directory_listing;
	std::vector<routeConfig> routes;
	std::vector<cgiConfig> cgis;
};



#endif
