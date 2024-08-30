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
	std::string handler;
	std::map<std::string, std::string> headers;
	std::map<std::string, std::string> files;
	bool		validmethod;
	userProfile userProfile;
};

struct ClientInfo
{
	int server_fd;
	HttpRequest request;
	std::string response;
	std::string requestBuffer;
	bool shouldclose;
	int outpipe;
	pid_t pid;
	ssize_t totalBytesSent;
	std::unordered_map<std::string, std::string> cgiEnv;

	ClientInfo() : server_fd(-1), shouldclose(false), outpipe(-1), pid(-1), totalBytesSent(0) {}
	ClientInfo(int fd) : server_fd(fd), shouldclose(false), outpipe(-1), pid(-1), totalBytesSent(0) {}
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
	std::string index;

	routeConfig() : directoryListing(false) {}
};

struct cgiConfig
{
	std::string extension;
	std::string handler;
	std::string script_alias;
	std::string root;
	bool		allowed;
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
