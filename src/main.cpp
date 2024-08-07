#include "HttpServer.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Wrong usage!" << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		config conf(av[1]);
		conf.begin();
		std::cout << conf << '\n';
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
