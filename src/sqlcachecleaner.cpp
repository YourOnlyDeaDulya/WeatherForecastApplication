#include "sqlcachecleaner.h"

SqlCacheCleaner::SqlCacheCleaner()
{

}

bool SqlCacheCleaner::TryCleanCache(const QString& db_name, QMutex* mutex)
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", "cleaner");
    db.setDatabaseName(db_name);
    if(!db.open())
    {
        return false;
    }

    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON;"))
    {
        return false;
    }

    if(!query.exec("DELETE FROM ForecastWeather WHERE request_date < DATE('now');"))
    {
        return false;
    }

    if(!query.exec("DELETE FROM CurrentWeather WHERE request_date < DATE('now');"))
    {
        return false;
    }

    query.finish();
    db.close();
    return true;
}
