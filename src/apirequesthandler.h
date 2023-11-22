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
#include "apiqueryconstructor.h"
#include "request_info_types.h"

class APIForecastServiceInterface //переименовать
{
public:

    virtual WeatherCollector GetRequestAnswer(const RequestDto& request, INFO_TYPE type) const = 0;
    virtual bool CheckForInternetConnection(QWidget *parent) const = 0;
    virtual ~APIForecastServiceInterface() = default;
};

class APIForecastService final : public APIForecastServiceInterface
{
public:
    explicit APIForecastService(QObject *parent);
    virtual WeatherCollector GetRequestAnswer(const RequestDto& query, INFO_TYPE type) const override;

    static QJsonDocument MakeRequest(const QString& query, QNetworkAccessManager* mNetManager);
    static WeatherCollector MakeCurrentWeatherRequest(const QString& query, QNetworkAccessManager* mNetManager); //API
    static WeatherCollector MakeForecastWeatherRequest(const QString& query, QNetworkAccessManager* mNetManager); //API

    bool CheckForInternetConnection(QWidget *parent) const override; //API

    static CurrentWeather MakeCurrentWeatherDto(const QVariantMap& map);
    static ForecastWeather MakeForecastWeatherDto(const QVariantMap& map);

private:

    std::unique_ptr<QNetworkAccessManager> mNetManager_;
    static std::unique_ptr<APIQueryConstructorInterface> query_constructor_;

    const QMap<INFO_TYPE, std::function<WeatherCollector(const QString& query, QNetworkAccessManager* mNetManager)>> request_type_to_function_
    {
        { INFO_TYPE::CURRENT, MakeCurrentWeatherRequest},
        { INFO_TYPE::FORECAST, MakeForecastWeatherRequest},
    };
};

#endif // APIREQUESTHANDLER_H
