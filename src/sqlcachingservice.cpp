#include "sqlcachingservice.h"

QString GetCurrentDate()
{
    QDate currentDate = QDate::currentDate();

    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    return QString::number(year) + "-" + QString::number(month) + "-" + QString::number(day);
}

SqlCachingService::SqlCachingService()
{
}

bool SqlCachingService::TryConnectToDataBase()
{
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("./../../sqlite_db/weather_request.db");
    return db_.open();
}

bool SqlCachingService::TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector) const
{
    if(!db_.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    return request_type_to_load_method_[type](w_collector);
}

bool SqlCachingService::TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector)
{
    if(!db_.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    return request_type_to_cache_method_[type](w_collector);
}

bool SqlCachingService::TryCacheCurrent(const WeatherCollector& w_collector)
{
    QSqlQuery query;
    if(!query.exec("DELETE FROM CurrentWeather"))
    {
        ErrorMessage("Не удалось очистить кэш: " + query.lastError().text());
        return false;
    }

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
    + "'" + GetCurrentDate() + "')";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlCachingService::TryCacheForecast(const WeatherCollector& w_collector)
{
    QSqlQuery query;
    if(!query.exec("DELETE FROM ForecastWeather"))
    {
        ErrorMessage("Не удалось очистить кэш: " + query.lastError().text());
        return false;
    }

    const auto forecast = w_collector.GerForecastWeather();


    QString query_text = "INSERT INTO ForecastWeather (city, country, request_date) VALUES (";
    query_text += "'" + forecast.city_ + "', "
                + "'" + forecast.country_ + "', "
                + "'" + GetCurrentDate() + "')";

    if(!query.exec(std::move(query_text)) || !TryCacheForecastDays(forecast))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlCachingService::TryCacheForecastDays(const ForecastWeather& forecast)
{
    QSqlQuery query;
    if(!query.exec("DELETE FROM ForecastDays"))
    {
        ErrorMessage("Не удалось очистить кэш: " + query.lastError().text());
        return false;
    }

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
            return false;
        }
    }

    return true;
}

bool SqlCachingService::TryLoadLastCurrentRequest(WeatherCollector& w_collector) const
{
    QSqlQuery query;
    QString query_text = "SELECT city, country, day, month, year, hour, minute, "
                         "weather_condition, celcium_t, feels_like, is_day "
                         "FROM CurrentWeather";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        return false;
    }

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

    //results_->setAnswer(w_collector);
    return true;
}

bool SqlCachingService::TryLoadLastForecastRequest(WeatherCollector& w_collector) const
{
    QSqlQuery query;
    QString query_text = "SELECT city, country FROM ForecastWeather";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        return false;
    }


    while(query.next())
    {
        ForecastWeather forecast;
        forecast.city_ = query.value(0).toString();
        forecast.country_ = query.value(1).toString();

        if(!TryLoadForecastdays(forecast))
        {
            return false;
        }

        w_collector.SetForecast(std::move(forecast));
    }

        //results_->setAnswer(w_collector);
    return true;
}

bool SqlCachingService::TryLoadForecastdays(ForecastWeather& forecast) const
{
    QSqlQuery query;
    QString query_text = "SELECT avg_t, min_t, max_t, "
                         "weather_condition, day, month, year "
                         "FROM ForecastDays";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось загрузить forecast_days: " + query.lastError().text());
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

    return true;
}
