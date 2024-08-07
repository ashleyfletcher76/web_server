#include "HttpServer.hpp"

volatile sig_atomic_t shutdownFlag = 0;

void	signalHandler(int signum)
{
	shutdownFlag = 1;
	(void)signum;
}


int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Wrong usage!" << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		signal(SIGINT, signalHandler);
		std::vector<struct pollfd> poll_fds;
		HttpServer sv(av[1], 8090, poll_fds);
		std::cout << sv.getConfig() << '\n';
		sv.begin();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
