#include "sqlcachecleaner.h"

SqlCacheCleaner::SqlCacheCleaner()
{

}

bool SqlCacheCleaner::TryCleanCache(const QString& db_name, QMutex* mutex)
{
    QMutexLocker locker(mutex);
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_name);
    if(!db.open())
    {
        ErrorMessage("Не удалось открыть базу данных");
        return false;
    }

    QSqlQuery query;
    if(!query.exec("PRAGMA foreign_keys = ON;"))
    {
        ErrorMessage("Не удалось установить pragma-расширение" + query.lastError().text());
        return false;
    }

    if(!query.exec("DELETE FROM ForecastWeather WHERE request_date < DATE('now');"))
    {
        ErrorMessage("Не удалось очистить кэш" + query.lastError().text());
        return false;
    }

    if(!query.exec("DELETE FROM CurrentWeather WHERE request_date < DATE('now');"))
    {
        ErrorMessage("Не удалось очистить кэш" + query.lastError().text());
        return false;
    }

    return true;
}
