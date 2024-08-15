#ifndef DATABASE_HPP
# define DATABASE_HPP

#include "includes.hpp"
#include "log.hpp"

class Database
{
	private:
		std::string	dbPath;
		Logger&		logger;
		sqlite3		*db;
		void	initDatabase();

	public:
		Database(const std::string& dbPath, Logger& logger);
		~Database();
};

#endif
