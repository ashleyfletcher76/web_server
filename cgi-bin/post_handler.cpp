#include <iostream>
#include <string>
#include <cstdlib>

int main()
{
	char *contentLengthStr = getenv("CONTENT_LENGTH");
	if (contentLengthStr == nullptr)
	{
		std::cerr << "Content-Length not set" << std::endl;
		return 1;
	}

	int contentLength = std::stoi(contentLengthStr);

	std::string postData;
	postData.resize(contentLength);
	std::cin.read(&postData[0], contentLength);

	std::cout << "am i here ? " << '\n';
	std::cout << "Content-type: text/html\r\n\r\n";
	std::cout << "<html><body><h1>Post Data Received</h1><p>" << postData << "</p></body></html>";

	return 0;
}