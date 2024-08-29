
#include "HttpServer.hpp"

void HttpServer::handleUpload(int client_socket, HttpRequest &request)
{
	if (request.files.empty())
	{
		std::cout << "No files were uploaded." << std::endl;
		return ;
	}
	for (const auto &file : request.files)
	{
		const std::string &filename = file.first;
		const std::string &fileData = file.second;

		std::ofstream outFile("./uploads/" + filename, std::ios::binary);
		if (outFile.is_open())
		{
			outFile.write(fileData.data(), fileData.size());
			outFile.close();
		}
	}

	const std::string responseBody = "<html><body>File uploaded succesfully!</body></html>";
	clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, 
		"OK", responseBody, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
	deregisterReadEvent(client_socket);
	registerWriteEvent(client_socket);
}