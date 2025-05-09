#pragma once
#include "DataUser.h"
#include <sqlite_orm/sqlite_orm.h>
#include <vector>
#include <optional>
#include <string>

namespace sql = sqlite_orm;

inline auto CreateStorage(const std::string &filename)
{
	return sql::make_storage(filename,
							 sql::make_table("players",
											 sqlite_orm::make_column("username", &DataUser::username, sqlite_orm::primary_key()),
											 sqlite_orm::make_column("score", &DataUser::score),
											 sqlite_orm::make_column("points", &DataUser::points),
											 sqlite_orm::make_column("canDoubleBulletSpeed", &DataUser::canDoubleBulletSpeed),
											 sqlite_orm::make_column("canReduceReloadTime", &DataUser::canReduceReloadTime)));
}

using Storage = decltype(CreateStorage(""));

class DatabaseManager
{

public:
	DatabaseManager();
	void AddUser(const DataUser &user);
	std::optional<DataUser> GetUser(const std::string &username);
	std::vector<DataUser> GetAllUsers();
	void UpdateUser(const DataUser &user);
	void DeleteUser(const std::string &username);

private:
	const std::string kDbFile{"Titans_vanguard.sqlite"};

private:
	Storage m_db = CreateStorage(kDbFile);
};