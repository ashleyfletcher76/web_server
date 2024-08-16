#include "HttpServer.hpp"

void	HttpServer::handlePostRequest(int client_socket)
{
	HttpRequest& request = clientInfoMap[client_socket].request;
	std::string responseBody;

	if (request.headers["content-type"] == "application/x-www-form-urlencoded")
	{
		std::map<std::string, std::string> formData;
		std::istringstream bodyStream(request.body);
		std::string pair;
		while (std::getline(bodyStream, pair, '&'))
		{
			size_t equals = pair.find('=');
			if (equals != std::string::npos)
			{
				std::string key = pair.substr(0, equals);
				std::string value = pair.substr(equals + 1);
				formData[key] = urlDecode(value);
			}
		}
		// log data into a file
		std::ofstream logFile("userLog.txt", std::ios::app);
		if (logFile.is_open())
		{
			logFile << "New POST request: \n";
			for (std::map<std::string, std::string>::const_iterator iter = formData.begin(); iter != formData.end(); iter++)
				logFile << iter->first << ": " << iter->second << std::endl;
			logFile << "--------------\n";
			logger.logMethod("INFO", "New log file updated");
			logFile.close();
			std::cout << "Here inside logFile success" << std::endl;
		}
		else
		{
			std::cout << "Here inside logFile fail" << std::endl;
			logger.logMethod("ERROR", "Could not open log file.");
		}
		// create response from form data
		responseBody = "<html><body>POST data recieved:<br>"; // change for an actual html file
		for (std::map<std::string, std::string>::const_iterator iter = formData.begin(); iter != formData.end(); iter++)
			responseBody += iter->first + ": " + iter->second + "<br>";
		responseBody += "</body></html>"; // change for an actual html file
	}
	else if (request.headers["Content-Type"] == "application/json")
		responseBody = "<html><body>JSON data recieved</body></html>";
	clientInfoMap[client_socket].response = formatHttpResponse(200, "OK", responseBody, clientInfoMap[client_socket].shouldclose);
	writeResponse(client_socket);
}
