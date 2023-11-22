#ifndef SQLCACHINGSERVICE_H
#define SQLCACHINGSERVICE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "weatherdatacollector.h"
#include "request_info_types.h"
#include "errormessage.h"
#include <QDate>
#include <QMutex>
#include <QMutexLocker>
#include <future>

QString GetCurrentDate();

class SqlCachingServiceInterface
{
public:
    virtual bool TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector) = 0;
    virtual bool TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector) const = 0;
    virtual bool TryConnectToDataBase() = 0;
    virtual ~SqlCachingServiceInterface() = default;
};

class SqlCachingService : public SqlCachingServiceInterface
{
public:
    SqlCachingService();

    virtual bool TryConnectToDataBase() override; //DB
    virtual bool TryLoadLastRequest(INFO_TYPE type, WeatherCollector& w_collector) const override;
    virtual bool TryCacheLastRequest(INFO_TYPE type, const WeatherCollector& w_collector) override;

private:
    QSqlDatabase db_;

    bool TryCacheCurrent(const WeatherCollector& w_collector);
    bool TryCacheForecast(const WeatherCollector& w_collector);
    bool TryCacheForecastDays(const ForecastWeather& forecast);

    bool TryLoadLastForecastRequest(WeatherCollector& w_collector) const;
    bool TryLoadLastCurrentRequest(WeatherCollector& w_collector) const;
    bool TryLoadForecastdays(ForecastWeather& forecast) const;



    const QMap<INFO_TYPE, std::function<bool(const WeatherCollector&)>> request_type_to_cache_method_
        {
            { INFO_TYPE::CURRENT, [&](const WeatherCollector& w_collector){ return TryCacheCurrent(w_collector); }},
            { INFO_TYPE::FORECAST, [&](const WeatherCollector& w_collector){ return TryCacheForecast(w_collector); }}
        };

    const QMap<INFO_TYPE, std::function<bool(WeatherCollector&)>> request_type_to_load_method_
        {
            { INFO_TYPE::CURRENT, [&](WeatherCollector& w_collector){ return TryLoadLastCurrentRequest(w_collector); }},
            { INFO_TYPE::FORECAST, [&](WeatherCollector& w_collector){ return TryLoadLastForecastRequest(w_collector); }}
        };
};

#endif // SQLCACHINGSERVICE_H
