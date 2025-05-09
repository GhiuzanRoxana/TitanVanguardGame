#include "DatabaseManager.h"

DatabaseManager::DatabaseManager()
{
    m_db.sync_schema();
}

void DatabaseManager::AddUser(const DataUser &user)
{

    m_db.replace(user);
}

std::optional<DataUser> DatabaseManager::GetUser(const std::string &username)
{
    try
    {

        auto user = m_db.get<DataUser>(username);
        return user;
    }
    catch (const std::system_error &e)
    {
        return std::nullopt;
    }
}

std::vector<DataUser> DatabaseManager::GetAllUsers()
{
    return m_db.get_all<DataUser>();
}

void DatabaseManager::UpdateUser(const DataUser &user)
{
    m_db.update(user);
}

void DatabaseManager::DeleteUser(const std::string &username)
{
    m_db.remove<DataUser>(username);
}
