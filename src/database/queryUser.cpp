#include "database.hpp"

bool	Database::fetchAllProfiles(std::vector<userProfile>& profiles)
{
	return (getAllProfiles(profiles));
}

bool	Database::getAllProfiles(std::vector<userProfile>& profiles)
{
	const char* sql = "SELECT id, name FROM Users";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to prepare all profiles query: " + std::string(sqlite3_errmsg(db)));
		return (false);
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		userProfile profile;
		profile.id = sqlite3_column_int(stmt, 0);
		profile.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		profiles.push_back(profile);
	}
	sqlite3_finalize(stmt);
	return (true);
}

bool	Database::executeQuery(const std::string& query, const std::vector<std::string>& params, userProfile& profile)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK)
	{
		logger.logMethod("ERROR", "Failed to query profile: " + std::string(sqlite3_errmsg(db)));
		return (false);
	}

	for (size_t i = 0; i < params.size(); ++i)
	{
		if (sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
		{
			logger.logMethod("ERROR", "Failed to bind parameter: " + std::to_string(i) + ": " + std::string(sqlite3_errmsg(db)));
			sqlite3_finalize(stmt);
			return (false);
		}
	}
	int stepResult = sqlite3_step(stmt);
	logger.logMethod("DEBUG", "sqlite3_step result: " + std::to_string(stepResult));
	if (stepResult == SQLITE_ROW)
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
	std::string query = "SELECT * FROM Users WHERE id = ?";
	std::vector<std::string> params = {name};
	std::cout << name << std::endl;
	return (executeQuery(query, params, profile));
}
