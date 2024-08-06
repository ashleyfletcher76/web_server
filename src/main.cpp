#include "HttpServer.hpp"

int	main()
{
	signal(SIGINT, signalHandler);
	std::vector<struct pollfd> poll_fds;

	HttpServer server(8090, poll_fds);

	try
	{
		server.begin();

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}

	return (0);
}
