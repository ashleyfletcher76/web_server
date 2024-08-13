#ifndef LOG_HPP
# define LOG_HPP

#include "includes.hpp"

class Logger
{
	private:
		std::string logFilename;
		std::ofstream logFile;

	public:
		Logger();
		~Logger();

	void	logMethod(const std::string& level, const std::string& msg, int client_socket);
};

#endif
