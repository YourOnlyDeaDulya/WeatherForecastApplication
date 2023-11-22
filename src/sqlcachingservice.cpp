#include "sqlcachingservice.h"

QString GetCurrentDate()
{
    QDate currentDate = QDate::currentDate();

    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    return QString::number(year) + "-" + QString::number(month) + "-" + QString::number(day);
}

static QSqlDatabase ConnectToDatabase(const QString& db_name, const QString& connection_name)
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", connection_name);
    db.setDatabaseName(db_name);

    if (!db.open()) {
        return QSqlDatabase();
    }

    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON;"))
    {
        ErrorMessage("Не удалось установить pragma-расширение" + query.lastError().text());
    }

    return db;
}

CachingService::CachingService()
{
}

bool CachingService::TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector, const QString& city) const
{
    return request_type_to_load_method_[type](w_collector, city);
}

bool CachingService::TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector)
{
    return request_type_to_cache_method_[type](w_collector);
}

bool CachingService::TryCacheCurrent(const WeatherCollector& w_collector)
{
    QSqlDatabase::removeDatabase("current_cache_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "current_cache_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);

    const auto current = w_collector.GetCurrentWeather();

    QString query_text = "INSERT INTO CurrentWeather (city, country, day, month, year, "
                         "hour, minute, weather_condition, celcium_t, feels_like, is_day, request_date) VALUES(";
    query_text
        +="'" + current.city_ + "', "
        + "'" + current.country_ + "', "
        + QString::number(current.date_.day_) + ", "
        + QString::number(current.date_.month_) + ", "
        + QString::number(current.date_.year_) + ", "
        + QString::number(current.hourly_weather_info_.time_.hour_) + ", "
        + QString::number(current.hourly_weather_info_.time_.minute_) + ", "
        + "'" + current.hourly_weather_info_.weather_condition_ + "', "
        + QString::number(current.hourly_weather_info_.celcium_t_) + ", "
        + QString::number(current.hourly_weather_info_.feels_like_t_) + ", "
        + QString::number(current.hourly_weather_info_.is_day_) + ", "
        + "'" + GetCurrentDate() + "') ";

    query_text
        += "ON CONFLICT (city) DO UPDATE SET "
        "day = excluded.day, month = excluded.month,"
        "year = excluded.year, hour = excluded.hour,"
        "hour = excluded.hour, minute = excluded.minute,"
        "weather_condition = excluded.weather_condition,"
        "celcium_t = excluded.celcium_t, feels_like = excluded.feels_like,"
        "is_day = excluded.is_day, request_date = excluded.request_date";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
        return false;
    }

    db.close();
    return true;
}

bool CachingService::TryCacheForecast(const WeatherCollector& w_collector)
{
    QSqlDatabase::removeDatabase("forecast_cache_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "forecast_cache_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);

    const auto forecast = w_collector.GerForecastWeather();

    if(!query.exec("DELETE FROM ForecastWeather WHERE city = '"
                    + forecast.city_ + "'"))
    {
        ErrorMessage("Не удалось очистить кэш: " + query.lastError().text());
        db.close();
        return false;
    }

    QString query_text = "INSERT INTO ForecastWeather (city, country, request_date) VALUES (";
    query_text
        += "'" + forecast.city_ + "', "
        + "'" + forecast.country_ + "', "
        + "'" + GetCurrentDate() + "')";

    if(!query.exec(std::move(query_text)) || !TryCacheForecastDays(forecast))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
        db.close();
        return false;
    }

    db.close();
    return true;
}

bool CachingService::TryCacheForecastDays(const ForecastWeather& forecast)
{
    QSqlDatabase::removeDatabase("forecast_days_cache_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "forecast_days_cache_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);

    const auto& days = forecast.forecast_days_;
    for(const auto& day : days)
    {
        QString query_text = "INSERT INTO ForecastDays (avg_t, min_t, max_t, "
                             "weather_condition, day, month, year, request_date, city) VALUES (";
        query_text
            += QString::number(day.avg_t) + ", "
            + QString::number(day.min_max_t_.first) + ", "
            + QString::number(day.min_max_t_.second) + ", '"
            + day.weather_condition_ + "', "
            + QString::number(day.date_.day_) + ", "
            + QString::number(day.date_.month_) + ", "
            + QString::number(day.date_.year_) + ", '"
            + GetCurrentDate() + "', '"
            + forecast.city_ + "')";

        if(!query.exec(std::move(query_text)))
        {
            ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
            db.close();
            return false;
        }
    }

    db.close();
    return true;
}

bool CachingService::TryLoadLastCurrentRequest(WeatherCollector& w_collector, const QString& city)
{
    QSqlDatabase::removeDatabase("current_load_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "current_load_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);

    QString query_text = "SELECT city, country, day, month, year, hour, minute, "
                         "weather_condition, celcium_t, feels_like, is_day "
                         "FROM CurrentWeather WHERE city = '" + city + "'";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        db.close();
        return false;
    }

    if(!query.next())
    {
        ErrorMessage("Информации по данному городу нет. Будет показан последний запрос");

        query_text = "SELECT city, country, day, month, year, hour, minute, "
                             "weather_condition, celcium_t, feels_like, is_day "
                             "FROM CurrentWeather ORDER BY city DESC LIMIT 1;";
        query.exec(std::move(query_text));

        if(!query.next())
        {
            ErrorMessage("Локальные данные отсутствуют");
            db.close();
            return false;
        }
    }

    query.previous();

    while(query.next())
    {
        CurrentWeather current;

        current.city_ = query.value(0).toString();
        current.country_ = query.value(1).toString();

        current.date_.day_ = query.value(2).toInt();
        current.date_.month_ = query.value(3).toInt();
        current.date_.year_ = query.value(4).toInt();

        current.hourly_weather_info_.time_.hour_ =
            query.value(5).toInt();
        current.hourly_weather_info_.time_.minute_ =
            query.value(6).toInt();

        current.hourly_weather_info_.weather_condition_ =
            query.value(7).toString();
        current.hourly_weather_info_.celcium_t_ =
            query.value(8).toDouble();
        current.hourly_weather_info_.feels_like_t_ =
            query.value(9).toDouble();
        current.hourly_weather_info_.is_day_ =
            query.value(10).toBool();

        w_collector.SetCurrent(std::move(current));
    }

    db.close();
    return true;
}

bool CachingService::TryLoadLastForecastRequest(WeatherCollector& w_collector, const QString& city)
{
    QSqlDatabase::removeDatabase("forecast_load_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "forecast_load_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);

    QString query_text = "SELECT city, country FROM ForecastWeather WHERE city = '" + city + "'";
    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        db.close();
        return false;
    }

    if(!query.next())
    {
        ErrorMessage("Информации по данному городу нет. Будет показан последний запрос");

        query_text = "SELECT city, country FROM ForecastWeather LIMIT 1";
        query.exec(std::move(query_text));

        if(!query.next())
        {
            ErrorMessage("Локальные данные отсутствуют");
            db.close();
            return false;
        }
    }

    query.previous();

    while(query.next())
    {
        ForecastWeather forecast;
        forecast.city_ = query.value(0).toString();
        forecast.country_ = query.value(1).toString();

        if(!TryLoadForecastdays(forecast))
        {
            db.close();
            return false;
        }

        w_collector.SetForecast(std::move(forecast));
    }

    db.close();
    return true;
}

bool CachingService::TryLoadForecastdays(ForecastWeather& forecast)
{
    QSqlDatabase::removeDatabase("forecast_load_conn");
    QSqlDatabase db = ConnectToDatabase(db_name_, "forecast_days_load_conn");

    if(!db.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    QSqlQuery query(db);
    QString query_text = "SELECT avg_t, min_t, max_t, "
                         "weather_condition, day, month, year "
                         "FROM ForecastDays WHERE city = '" + forecast.city_ + "'";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить forecast_days: " + query.lastError().text());
        db.close();
        return false;
    }

    while(query.next())
    {
        ForecastDay day;

        day.avg_t = query.value(0).toDouble();
        day.min_max_t_.first = query.value(1).toDouble();
        day.min_max_t_.second = query.value(2).toDouble();

        day.weather_condition_ = query.value(3).toString();

        day.date_.day_ = query.value(4).toInt();
        day.date_.month_ = query.value(5).toInt();
        day.date_.year_ = query.value(6).toInt();

        forecast.forecast_days_.push_back(std::move(day));
    }

    db.close();
    return true;
}

QString CachingService::GetDBName() const
{
    return db_name_;
}

bool CachingService::TryCleanCache()
{
    if(!SqlCacheCleaner::TryCleanCache(std::ref(db_name_)))
    {
        return false;
    }

    return true;
}
