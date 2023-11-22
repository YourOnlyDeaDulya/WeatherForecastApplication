#ifndef SQLCAHCECLEANER_H
#define SQLCAHCECLEANER_H


//Cделать класс, проверяющий на валидность кэша дате и удаляющий устаревший кэш
//Cделать модифицирование базы данных асинхронным
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QMutexLocker>

#include "errormessage.h"

class SqlCacheCleaner
{
public:
    SqlCacheCleaner();

    static bool TryCleanCache(const QString& db_name, QMutex* mutex);
};

#endif // SQLCAHCECLEANER_H
