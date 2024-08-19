#include "database.hpp"

bool	Database::handleDeleteProfile(const std::string& id)
{
	userProfile profile;
	if (getUserProfile(id, profile))
		return (deleteProfile(std::stoi(id)));
	logger.logMethod("ERROR", "Profile ID mpt found: " + id);
	return (false);
}

bool	Database::deleteProfile(int profileID)
{
	const char* sqlDelete = "DELETE FROM Users WHERE ID = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sqlDelete, -1, &stmt, nullptr) != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to make delete statement: " + std::string(sqlite3_errmsg(db)));
		return (false);
	}
	sqlite3_bind_int(stmt, 1, profileID);
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		logger.logMethod("ERROR", "Failed to execute delete statement: " + std::string(sqlite3_errmsg(db)));
		sqlite3_finalize(stmt);
		return (false);
	}
	sqlite3_finalize(stmt);
	logger.logMethod("INFO", "Successfully deleted profile with ID: " + std::to_string(profileID));
	return (true);
}
