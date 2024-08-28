#include "HttpServer.hpp"

bool	HttpServer::checkIfCgiAllowed(const std::string& uri, int client_socket)
{
	auto serverIt = servers.find(clientInfoMap[client_socket].server_fd);
	const serverInfo &srvInfo = serverIt->second->getServerInfo();

	std::string	fileExtension;
	std::size_t	pos = uri.find_last_of('.');

	if (pos != std::string::npos)
		fileExtension = uri.substr(pos);
	else
		return (false);

	std::cout << "Extracted file extension: " << fileExtension << "\nURI: " << uri << std::endl;
	
	for (const auto& cgi : srvInfo.cgis)
	{
		if (fileExtension == cgi.extension)
		{
			std::cout << "Here after serverinfo" << std::endl;
			return (true);
		}
	}
	return (false);
}
