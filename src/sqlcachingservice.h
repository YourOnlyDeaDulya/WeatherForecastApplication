#ifndef SQLCACHINGSERVICE_H
#define SQLCACHINGSERVICE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "weatherdatacollector.h"
#include "request_info_types.h"
#include "errormessage.h"
#include "sqlcachecleaner.h"

#include <QDate>
#include <QMutex>
#include <QMutexLocker>
#include <thread>

QString GetCurrentDate();

class SqlCachingServiceInterface
{
public:
    virtual bool TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector) = 0;
    virtual bool TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector, const QString& city) const = 0;
    virtual bool TryConnectToDataBase() = 0;
    virtual QString GetDBName() const = 0;
    virtual bool TryCleanCache() = 0;
    virtual ~SqlCachingServiceInterface() = default;
};

class SqlCachingService : public SqlCachingServiceInterface
{
public:
    SqlCachingService();

    virtual bool TryConnectToDataBase() override; //DB
    virtual bool TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector, const QString& city) const override;
    virtual bool TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector) override;
    virtual QString GetDBName() const override;
    virtual bool TryCleanCache() override;

private:
    QSqlDatabase db_;
    QMutex mutex;
    const QString db_name_ = "./../../sqlite_db/weather_request.db";

    bool TryCacheCurrent(const WeatherCollector& w_collector);
    bool TryCacheForecast(const WeatherCollector& w_collector);
    bool TryCacheForecastDays(const ForecastWeather& forecast);

    bool TryLoadLastForecastRequest(WeatherCollector& w_collector, const QString& city);
    bool TryLoadLastCurrentRequest(WeatherCollector& w_collector, const QString& city);
    bool TryLoadForecastdays(ForecastWeather& forecast);

    const QMap<INFO_TYPE, std::function<bool(const WeatherCollector&)>> request_type_to_cache_method_
        {
            { INFO_TYPE::CURRENT, [&](const WeatherCollector& w_collector){ return TryCacheCurrent(w_collector); }},
            { INFO_TYPE::FORECAST, [&](const WeatherCollector& w_collector){ return TryCacheForecast(w_collector); }}
        };

    const QMap<INFO_TYPE, std::function<bool(WeatherCollector&, const QString&)>> request_type_to_load_method_
        {
            { INFO_TYPE::CURRENT, [&](WeatherCollector& w_collector, const QString& city){ return TryLoadLastCurrentRequest(w_collector, city); }},
            { INFO_TYPE::FORECAST, [&](WeatherCollector& w_collector, const QString& city){ return TryLoadLastForecastRequest(w_collector, city); }}
        };
};

#endif // SQLCACHINGSERVICE_H
