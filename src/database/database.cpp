#include "database.hpp"

Database::Database(Logger& logger) : dbPath("/userdb.db"), logger(logger), db(nullptr)
{
	dbPath = "./uploads/profiles.userDB.db";

	if (mkdir("./uploads", 0755) == -1 && errno != EEXIST)
	{
		logger.logMethod("ERROR", "Failed to create uploads directory.");
		return ;
	}
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
	int	result = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
	bool dbExists = (result == SQLITE_OK);
	sqlite3_close(db);

	result = sqlite3_open(dbPath.c_str(), &db);
	if (result != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Cannot open database: " + std::string(sqlite3_errmsg(db)));
		sqlite3_close(db);
		db = nullptr;
		return ;
	}
	if (!dbExists)
	{
		logger.logMethod("INFO", "Creating a table...");
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
	else
		logger.logMethod("INFO", "Reusing valid database.");
}
