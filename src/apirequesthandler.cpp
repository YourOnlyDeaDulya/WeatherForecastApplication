#include "apirequesthandler.h"

void APIQueryModel::SetMainQuery(const QString& str)
{
    main_query_ = "&q=" + str;
}

void APIQueryModel::SetDaysQuery(const QString& str)
{
    days_query_ = "&days=" + str;
}


QString APIQueryModel::GetQuery(INFO_TYPE request_type) const
{
    QString result_query = request_opening_.value(request_type);

    const auto& methods_list = mandatory_methods_list_.value(request_type);
    for(auto it = methods_list.begin(); it != methods_list.end(); ++it)
    {
        result_query += *it;
    }

    return result_query;
}

QString APIQueryModel::GetMainQuery() const
{
    return main_query_;
}

QString APIQueryModel::GetDaysQuery() const
{
    return days_query_;
}

APIRequestModel::APIRequestModel(QObject *parent) : mNetManager(std::make_unique<QNetworkAccessManager>(parent))
{

}

QJsonDocument APIRequestModel::MakeRequest(const QString& query)
{
    QUrl url = QUrl(query);
    QNetworkRequest req(url);
    std::unique_ptr<QNetworkReply> reply;
    reply.reset(mNetManager->get(req));
    QEventLoop loop;
    QObject::connect(reply.get(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString jsonString = reply->readAll();
    return QJsonDocument::fromJson(jsonString.toUtf8());
}

WeatherCollector APIRequestModel::MakeCurrentWeatherRequest(const QString& query)
{
    QVariantMap map = MakeRequest(query).object().toVariantMap();

    if(map.contains("error"))
    {
        ErrorMessage(map["error"].toJsonObject().toVariantMap()["message"].toString());
        return WeatherCollector();
    }

    WeatherCollector w_collector;
    w_collector.SetCurrent(MakeCurrentWeather(map)); //Заняться ренеймингом.

    return w_collector;
}

WeatherCollector APIRequestModel::MakeForecastWeatherRequest(const QString& query)
{
    /*if(ui->daysCountInput->text().toInt() < 1 || ui->daysCountInput->text().toInt() > 10)
    {
        ErrorMessage("Введите корректное число дней");
        return WeatherCollector();
    }*/

    QVariantMap map = MakeRequest(query).object().toVariantMap();

    if(map.contains("error"))
    {
        ErrorMessage(map["error"].toJsonObject().toVariantMap()["message"].toString());
        return WeatherCollector();
    }

    WeatherCollector w_collector;
    w_collector.SetForecast(MakeForecastWeather(map));

    return w_collector;
}

CurrentWeather APIRequestModel::MakeCurrentWeather(const QVariantMap& map)
{
    const QVariantMap& location = map["location"].toJsonObject().toVariantMap();
    const QVariantMap& current = map["current"].toJsonObject().toVariantMap();

    CurrentWeather current_weather;
    current_weather.city_ = location["name"].toString();
    current_weather.country_ = location["country"].toString();

    QPair<Date, Time> dt = ParseDateTimeFromString(location["localtime"].toString());
    current_weather.date_ = dt.first;
    current_weather.hourly_weather_info_.time_ = dt.second;

    current_weather.hourly_weather_info_.celcium_t_ = current["temp_c"].toDouble();
    current_weather.hourly_weather_info_.feels_like_t_ = current["feelslike_c"].toDouble();
    current_weather.hourly_weather_info_.is_day_ = current["is_day"].toBool();
    current_weather.hourly_weather_info_.weather_condition_ = current["condition"].toJsonObject().toVariantMap()["text"].toString();

    return current_weather;
}

ForecastWeather APIRequestModel::MakeForecastWeather(const QVariantMap& map)
{
    const QVariantMap& location = map["location"].toJsonObject().toVariantMap();
    const QVariantMap& forecast = map["forecast"].toJsonObject().toVariantMap();

    ForecastWeather forecast_weather;
    forecast_weather.city_ = location["name"].toString();
    forecast_weather.country_ = location["country"].toString();

    const auto& forecast_days = forecast["forecastday"].toJsonArray();
    for(const auto& json_day : forecast_days)
    {
        ForecastDay forecast_day;
        const auto& day_map = json_day.toObject().toVariantMap();

        forecast_day.date_ = ParseDateFromString(day_map["date"].toString());

        const auto& day_info = day_map["day"].toJsonObject().toVariantMap();

        forecast_day.avg_t = day_info["avgtemp_c"].toDouble();

        double min_t = day_info["mintemp_c"].toDouble();
        double max_t = day_info["maxtemp_c"].toDouble();
        forecast_day.min_max_t_ = {min_t, max_t};

        forecast_day.weather_condition_ = day_info["condition"].toJsonObject().toVariantMap()["text"].toString();

        forecast_weather.forecast_days_.push_back(std::move(forecast_day));
    }

    return forecast_weather;
}

bool APIRequestModel::CheckForInternetConnection(QWidget *parent) const
{
    std::unique_ptr<QTcpSocket> sock = std::make_unique<QTcpSocket>(parent);
    sock->connectToHost("www.google.com", 80);
    bool connected = sock->waitForConnected(30000);

    if (!connected)
    {
        sock->abort();
        ErrorMessage("Отсутствует интернет-соединение. Будет показан последний запрос");
        return false;
    }
    sock->close();
    return true;
}
