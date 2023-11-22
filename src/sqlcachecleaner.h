#ifndef SQLCAHCECLEANER_H
#define SQLCAHCECLEANER_H


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
    static bool TryCleanCache(const QString& db_name);
};

#endif // SQLCAHCECLEANER_H
