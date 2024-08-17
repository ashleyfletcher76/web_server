#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "includes.hpp"

struct isNotSpace
{
	bool operator()(unsigned char ch) const
	{
		return (!(std::isspace(ch) || ch == '\r' || ch == '\n'));
	}
};

struct userProfile
{
	int			id;
	std::string	name;
	std::string	email;
	std::string	phoneNum;
	std::string	description;
};

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string body;
	std::string version;
	std::map<std::string, std::string> headers;
	userProfile	userProfile;
};

struct ClientInfo
{
	int server_fd;
	HttpRequest request;
	std::string response;
	bool shouldclose;

	ClientInfo() : server_fd(-1), shouldclose(false) {}
	ClientInfo(int fd) : server_fd(fd), shouldclose(false) {}
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
	int listen;
	std::string host;
	std::string server_name;
	std::string document_root;
	std::string default_file;
	std::string client_max_body_size;
	std::string directory_listing;
	std::vector<routeConfig> routes;
	std::vector<cgiConfig> cgis;
	std::unordered_map<int, std::string> errorPages;
};

#endif
