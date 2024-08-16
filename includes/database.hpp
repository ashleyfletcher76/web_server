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
		bool	insertUser(const std::string& name, const std::string& email,
			const std::string& phone, const std::string& description);

	public:
		Database(Logger& logger);
		~Database();
};

#endif
