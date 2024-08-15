#include "database.hpp"

Database::Database(const std::string& dbPath, Logger& logger) : dbPath(+ "/userdb.db"), logger(logger), db(nullptr)
{
	initDatabase();
}

Database::~Database()
{
	if (db)
	{
		sqlite3_close(db);
		logger.logMethod("INFO", "Database closed.");
	}
}

void	Database::initDatabase()
{
	if (!db)
	{
		logger.logMethod("ERROR", "Database is not opened.");
		return ;
	}
	if (sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK)
	{
		const char* sqlCreateTable = "CREATE TABLE IF NOT EXISTS Users ("
							"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
							"Name TEXT NOT NULL, "
							"Email TEXT NOT NULL, "
							"PhoneNumber TEXT NOT NULL, "
							"Description TEXT);";
		char* errMsg;
		if (sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
		{
			sqlite3_free(errMsg);
			logger.logMethod("ERROR", "Failed to create table: " + std::string(errMsg));
		}
		else
			logger.logMethod("INFO", "Table created successfully.");

	}
}
