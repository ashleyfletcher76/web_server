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
	int id;
	std::string name;
	std::string email;
	std::string phoneNum;
	std::string description;
};

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string body;
	std::string version;
	std::map<std::string, std::string> headers;
	userProfile userProfile;
};

struct ClientInfo
{
	int server_fd;
	HttpRequest request;
	std::string response;
	bool shouldclose;
	bool error;

	ClientInfo() : server_fd(-1), shouldclose(false), error(false) {}
	ClientInfo(int fd) : server_fd(fd), shouldclose(false), error(false) {}
};

struct routeConfig
{
	std::vector<std::string> allowedMethods;
	std::string path;
	bool directoryListing;
	std::string handler;
	std::string redirect;
	std::string rootDirectory;
	std::string defaultFile;

	routeConfig() : directoryListing(false) {}
};

struct cgiConfig
{
	std::string extension;
	std::string handler;
};

struct serverInfo
{
	int listen;
	int body_size;
	bool directory_listing;
	std::string host;
	std::string server_name;
	std::string document_root;
	std::string default_file;
	std::vector<cgiConfig> cgis;
	std::unordered_map<int, std::string> errorPages;
	std::unordered_map<std::string, routeConfig> routes;

	serverInfo() : listen(0), body_size(-1), directory_listing(false) {}
};

#endif
