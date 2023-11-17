#ifndef APIREQUESTHANDLER_H
#define APIREQUESTHANDLER_H

#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include "weatherdatacollector.h"
#include "errormessage.h"

enum class INFO_TYPE
{
    CURRENT,
    FORECAST,
};

class APIQueryView
{
public:
    virtual void SetMainQuery(const QString& str) = 0;
    virtual void SetDaysQuery(const QString& str) = 0;

    virtual QString GetQuery(INFO_TYPE request_type) const = 0;

    virtual ~APIQueryView() = default;
};

class APIQueryModel final : public APIQueryView
{
private:

    const QString API_CODE_ = "acff18c1162c419a925183952230511";
    const QString END_STATEMENT_ = "&aqi=no&lang=ru"; //Пока что обязательный конец запроса.
                                                      //Можно модифицировать, подобно обязательным методам,
                                                      //Чтобы передавать список запросов на добавление.

    const QMap<INFO_TYPE, QVector<QString>> mandatory_methods_list_
        {
         {INFO_TYPE::CURRENT, {GetMainQuery()}},
         {INFO_TYPE::FORECAST, {GetMainQuery(), GetDaysQuery()}},
        };

    const QMap<INFO_TYPE, QString> request_opening_
        {
         { INFO_TYPE::CURRENT, "http://api.weatherapi.com/v1/current.json?key=" + API_CODE_ },
         { INFO_TYPE::FORECAST, "http://api.weatherapi.com/v1/forecast.json?key=" + API_CODE_ },
         };

public:
    virtual void SetMainQuery(const QString& str) override;
    virtual void SetDaysQuery(const QString& str) override;

    virtual QString GetQuery(INFO_TYPE request_type) const override;

    QString GetMainQuery() const;
    QString GetDaysQuery() const;
private:
    QString main_query_;
    QString days_query_;
};

class APIRequestView
{
public:
    virtual void SetQuery(const QString& query) = 0;
    virtual WeatherCollector GetRequestAnswer(const QString& query, INFO_TYPE type) const = 0;
    virtual bool CheckForInternetConnection(QWidget *parent) const = 0;
    virtual ~APIRequestView() = default;
};

class APIRequestModel final : public APIRequestView
{
public:
    explicit APIRequestModel(QObject *parent);
    virtual void SetQuery(const QString& query) override;
    virtual WeatherCollector GetRequestAnswer(const QString& query, INFO_TYPE type) const override;

    QJsonDocument MakeRequest(const QString& query);
    WeatherCollector MakeCurrentWeatherRequest(const QString& query); //API
    WeatherCollector MakeForecastWeatherRequest(const QString& query); //API

    bool CheckForInternetConnection(QWidget *parent) const override; //API

    CurrentWeather MakeCurrentWeather(const QVariantMap& map);
    ForecastWeather MakeForecastWeather(const QVariantMap& map);

private:
    QString query_ = "";

    const QMap<INFO_TYPE, WeatherCollector> request_type_to_function_
    {
        { INFO_TYPE::CURRENT, MakeCurrentWeatherRequest(query_)},
        { INFO_TYPE::FORECAST, MakeForecastWeatherRequest(query_)},
    };

    std::unique_ptr<QNetworkAccessManager> mNetManager;
};

#endif // APIREQUESTHANDLER_H
