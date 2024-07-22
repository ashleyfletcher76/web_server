#include "HttpServer.hpp"

int	main()
{
	HttpServer server(8080);

	server.begin();
	return (0);
}
