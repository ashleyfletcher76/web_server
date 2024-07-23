#include "HttpServer.hpp"

void	HttpServer::sendErrorResponse(int statusCode, const std::string& reasonPhrase)
{
	std::string	response;
	 std::string htmlContent = "<html><head><title>Error</title></head><body><h1>"
							+ std::to_string(statusCode) + " " + reasonPhrase
							+ "</h1><p>The requested method is not supported.</p></body></html>";

	response = "HTTP/1.1 " + std::to_string(statusCode) + " " + reasonPhrase + "\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + std::to_string(htmlContent.size()) + "\r\n"
				"Connection: close\r\n\r\n"
				+ htmlContent;
	write(new_socket, response.c_str(), response.size());
	std::cout << "Error response sent" << std::endl;
}
