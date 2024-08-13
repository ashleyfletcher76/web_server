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
	Logger logger;
	try
	{
		signal(SIGINT, signalHandler);
		HttpServer sv(av[1], logger);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
