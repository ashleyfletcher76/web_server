#include <iostream>
#include <string>
#include <stdlib.h>

int main() 
{
	std::cout << "Content-type:text/html" << "\n" << std::endl;
	std::cout << "<html>" << std::endl;
	std::cout << "<head>" << std::endl;
	std::cout << "<title>Hello World - First CGI Program</title>" << std::endl;
	std::cout << "</head>" << std::endl;
	std::cout << "<body>" << std::endl;
	std::cout << "<h2>Hello World! This is my first CGI program</h2>" << std::endl;
	std::cout << "</body>" << std::endl;
	std::cout << "</html>" << std::endl;

	return (0);
}
