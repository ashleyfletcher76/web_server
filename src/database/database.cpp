#include "database.hpp"

void	Database::ensureDirectoryExist(const std::string& filePath)
{
	size_t pos = filePath.find_last_not_of('/');
	if (pos != std::string::npos)
	{
		std::string dirPath = filePath.substr(0, pos);
		struct stat st;
		if (stat(dirPath.c_str(), &st) == -1)
		{
			mkdir(dirPath.c_str(), 0700);
			ensureDirectoryExist(dirPath);
		}
	}
}

Database::Database(const std::string& dbPath, Logger& logger) : dbPath(dbPath), logger(logger), db(nullptr)
{
	ensureDirectoryExist(dbPath);
	int rc = sqlite3_open(dbPath.c_str(), &db);
	if (rc)
	{
		logger.logMethod("ERROR", "Error opening SQLite3 database: " + std::string(sqlite3_errmsg(db)), NOSTATUS);
		sqlite3_close(db);
		db = nullptr;
	}
	else
		logger.logMethod("INFO", "Opening database successfully.", NOSTATUS);
}

Database::~Database()
{
	if (db)
	{
		sqlite3_close(db);
		logger.logMethod("INFO", "Database closed.", NOSTATUS);
	}
}

void	Database::createTable()
{
	if (!db)
	{
		logger.logMethod("ERROR", "Database is not opened.", NOSTATUS);
		return ;
	}
	const char* sqlCreateTable =
		"CREATE TABLE IF NOT EXISTS Users ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"Username TEXT NOT NULL, "
		"Email TEXT NOT NULL UNIQUE);";

	char* errMsg = nullptr;
	int rc = sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to create table: " + std::string(errMsg), NOSTATUS);
		sqlite3_free(errMsg);
	}
	else
		logger.logMethod("INFO", "Table created successfully.", NOSTATUS);
}
