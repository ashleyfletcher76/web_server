#include "HttpServer.hpp"

void	HttpServer::trim(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), isNotSpace()));
	str.erase(std::find_if(str.rbegin(), str.rend(), isNotSpace()).base(), str.end());
}

int	HttpServer::getMaxClientBodySize(int client_socket)
{
	int server_fd = clientInfoMap[client_socket].server_fd;
	int maxSize = servers[server_fd]->getServerInfo().body_size;
	return (maxSize);
}
