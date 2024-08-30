
#include "HttpServer.hpp"

void HttpServer::handleUpload(int client_socket, HttpRequest &request)
{
	std::string uploaddirect = "./uploads/";

	if (isDirectory(request.handler))
		uploaddirect = request.handler;
	if (request.files.empty())
	{
		return ;
	}
	for (const auto &file : request.files)
	{
		const std::string &filename = file.first;
		const std::string &fileData = file.second;

		std::string path = uploaddirect + filename;
		std::ofstream outFile(path, std::ios::binary);

		if (outFile.is_open())
		{
			outFile.write(fileData.data(), fileData.size());
			outFile.close();
		}
	}

	const std::string responseBody = "<html><body>File uploaded succesfully!</body></html>";
	clientResponse[client_socket] = formatHttpResponse(clientInfoMap[client_socket].request.version, 200, 
		"OK", responseBody, clientInfoMap[client_socket].shouldclose, clientInfoMap[client_socket].request.uri);
	registerWriteEvent(client_socket);
}