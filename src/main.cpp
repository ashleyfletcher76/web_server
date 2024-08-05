#include "HttpServer.hpp"

int	main()
{
	std::vector<struct pollfd> poll_fds;

	HttpServer server(8080, poll_fds);

	server.begin();
	return (0);
}
