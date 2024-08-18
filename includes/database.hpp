#ifndef DATABASE_HPP
# define DATABASE_HPP

#include "includes.hpp"
#include "log.hpp"
#include "structs.hpp"

class Database
{
	private:
		std::string	dbPath;
		Logger&		logger;
		sqlite3		*db;
		void	initDatabase();
		bool	insertUser(const std::string& name, const std::string& email,
			const std::string& phone, const std::string& description);
		bool	executeQuery(const std::string& query, const std::vector<std::string>& params, userProfile& profile);
		bool	getAllProfiles(std::vector<userProfile>& profiles);

	public:
		Database(Logger& logger);
		~Database();
		bool	addUser(const std::string& name, const std::string& email,
			const std::string& phone, const std::string& description);
		bool	getUserProfile(const std::string& name, userProfile& profile);
		bool	fetchAllProfiles(std::vector<userProfile>& profiles);
};

#endif
