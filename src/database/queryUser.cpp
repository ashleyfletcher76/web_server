#include "database.hpp"

bool	Database::executeQuery(const std::string& query, const std::vector<std::string>& params, userProfile& profile)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to query profile: " + std::string(sqlite3_errmsg(db)));
		return (false);
	}

	for (std::size_t i = 0; i < params.size(); i++)
		sqlite3_bind_text(stmt, 1 + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		profile.id = sqlite3_column_int(stmt, 0);
		profile.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		profile.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		profile.phoneNum = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		profile.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		sqlite3_finalize(stmt);
		return (true);
	}
	sqlite3_finalize(stmt);
	return (false);
}

bool	Database::getUserProfile(const std::string& name, userProfile& profile)
{
	std::string query = "SELECT * FROM Users WHERE Name = ?";
	std::vector<std::string> params = {name};
	return (executeQuery(query, params, profile));
}
