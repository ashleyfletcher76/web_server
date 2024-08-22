#include "log.hpp"

void	Logger::logMethod(const std::string& level, const std::string& msg)
{
	std::time_t currentTime = std::time(0);
	std::tm* localTime = std::localtime(&currentTime);

	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", localTime);  // Underscore instead of spaces

	if (logFile.is_open())
		logFile << timestamp << " [" << level << "]" << " - " << msg << std::endl;
	else
		std::cerr << "Unable to open log file" << std::endl;
	//std::cout << timestamp << " [" << level << "]" << " - " << msg  << std::endl;
}

Logger::Logger()
{
	if (system("mkdir -p logs") != 0)
		std::cerr << "Failed to create logs directory" << std::endl;

	std::time_t currentTime = std::time(0);
	std::tm* localTime = std::localtime(&currentTime);

	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", localTime);  // Underscore instead of spaces
	logFilename = "logs/log_" + std::string(timestamp) + ".txt";

	logFile.open(logFilename, std::ios_base::app);
	if (!logFile.is_open())
		std::cerr << "Unable to open log file: " << logFilename << std::endl;
	else
		logMethod("INFO", "Log started");
}

Logger::~Logger()
{
	if (logFile.is_open())
	{
		logMethod("INFO", "Log closed.");
		logFile.close();
	}
}
