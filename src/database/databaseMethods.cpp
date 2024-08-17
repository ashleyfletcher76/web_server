#include "database.hpp"

bool	Database::addUser(const std::string& name, const std::string& email, const std::string& phone, const std::string& description)
{
	if (name.empty() || email.empty() || phone.empty() || description.empty())
	{
		logger.logMethod("ERROR", "Attempting to add data that is incomplete.");
		return (false);
	}
	return (insertUser(name, email, phone, description));
}

bool	Database::insertUser(const std::string& name, const std::string& email, const std::string& phone, const std::string& description)
{
	const char* sqlInsert = "INSERT INTO Users (Name, Email, PhoneNumber, Description) VALUES (?, ?, ?, ?);";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, NULL) != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to prepare insert statement: " + std::string(sqlite3_errmsg(db)));
		return (false);
	}

	// bind values SQL statement
	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, phone.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_TRANSIENT);

	// execute the prepared insertion
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		logger.logMethod("ERROR", "Failed to execute the insert data: " + std::string(sqlite3_errmsg(db)));
		sqlite3_finalize(stmt);
		return (false);
	}
	sqlite3_finalize(stmt);
	logger.logMethod("INFO", "User inserted successfully.");
	return (true);
}
