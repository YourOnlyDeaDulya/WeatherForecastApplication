#include "weatherdatacollector.h"

QPair<Date, Time> ParseDateTimeFromString(const QString& str)
{
    static QRegularExpression regex("(\\d{4})-(\\d{1,2})-(\\d{1,2}) (\\d{1,2}):(\\d{2})");
    QRegularExpressionMatch match = regex.match(str);

    Date date;
    date.year_ = match.captured(1).toInt();
    date.month_ = match.captured(2).toInt();
    date.day_ = match.captured(3).toInt();

    Time time;
    time.hour_ = match.captured(4).toInt();
    time.minute_ = match.captured(5).toInt();

    return QPair<Date, Time>{date, time};
}

Date ParseDateFromString(const QString& str)
{
    static QRegularExpression regex("(\\d{4})-(\\d{1,2})-(\\d{1,2})");
    QRegularExpressionMatch match = regex.match(str);

    return Date{match.captured(3).toInt(), match.captured(2).toInt(), match.captured(1).toInt()};
}

HourlyWeather::HourlyWeather(const HourlyWeather& other)
{
    *this = other;
}

HourlyWeather& HourlyWeather::operator=(const HourlyWeather& other)
{
    if(this == &other)
    {
        return *this;
    }

    time_.hour_ = other.time_.hour_;
    time_.minute_ = other.time_.minute_;
    weather_condition_ = other.weather_condition_;
    celcium_t_ = other.celcium_t_;
    feels_like_t_ = other.feels_like_t_;
    is_day_ = other.is_day_;

    return *this;
}

HourlyWeather::HourlyWeather(HourlyWeather&& other)
{
    *this = std::move(other);
}

HourlyWeather& HourlyWeather::operator=(HourlyWeather&& other)
{
    if(this == &other)
    {
        return *this;
    }

    time_.hour_ = other.time_.hour_;
    time_.minute_ = other.time_.minute_;
    weather_condition_ = std::move(other.weather_condition_);
    celcium_t_ = other.celcium_t_;
    feels_like_t_ = other.feels_like_t_;
    is_day_ = other.is_day_;

    return *this;
}

CurrentWeather::CurrentWeather(const CurrentWeather& other)
{
    *this = other;
}

CurrentWeather& CurrentWeather::operator=(const CurrentWeather& other)
{
    if(this == &other)
    {
        return *this;
    }

    city_ = other.city_;
    country_ = other.country_;
    date_.day_ = other.date_.day_;
    date_.month_ = other.date_.month_;
    date_.year_ = other.date_.year_;
    hourly_weather_info_ = other.hourly_weather_info_;

    return *this;
}

CurrentWeather::CurrentWeather(CurrentWeather&& other)
{
    *this = std::move(other);
}

CurrentWeather& CurrentWeather::operator=(CurrentWeather&& other)
{
    if(this == &other)
    {
        return *this;
    }

    city_ = std::move(other.city_);
    country_ = std::move(other.country_);
    date_.day_ = other.date_.day_;
    date_.month_ = other.date_.month_;
    date_.year_ = other.date_.year_;
    hourly_weather_info_ = std::move(other.hourly_weather_info_);

    return *this;
}

ForecastDay::ForecastDay(const ForecastDay& other)
{
    *this = other;
}
ForecastDay::ForecastDay(ForecastDay&& other)
{
    *this = std::move(other);
}

ForecastDay& ForecastDay::operator=(const ForecastDay& other)
{
    if(this == &other)
    {
        return *this;
    }

    min_max_t_ = other.min_max_t_;
    avg_t = other.avg_t;
    weather_condition_ = other.weather_condition_;
    date_.day_ = other.date_.day_;
    date_.month_ = other.date_.month_;
    date_.year_ = other.date_.year_;

    return *this;
}
ForecastDay& ForecastDay::operator=(ForecastDay&& other)
{
    if(this == &other)
    {
        return *this;
    }

    min_max_t_ = other.min_max_t_;
    avg_t = other.avg_t;
    weather_condition_ = std::move(other.weather_condition_);
    date_.day_ = other.date_.day_;
    date_.month_ = other.date_.month_;
    date_.year_ = other.date_.year_;

    return *this;
}

ForecastWeather::ForecastWeather(const ForecastWeather& other)
{
    *this = other;
}

ForecastWeather& ForecastWeather::operator=(const ForecastWeather& other)
{
    if(this == &other)
    {
        return *this;
    }

    city_ = other.city_;
    country_ = other.country_;
    forecast_days_ = other.forecast_days_;

    return *this;
}

ForecastWeather::ForecastWeather(ForecastWeather&& other)
{
    *this = std::move(other);
}

ForecastWeather& ForecastWeather::operator=(ForecastWeather&& other)
{
    if(this == &other)
    {
        return *this;
    }

    city_ = std::move(other.city_);
    country_ = std::move(other.country_);
    forecast_days_ = std::move(other.forecast_days_);


    return *this;
}

void WeatherCollector::SetCurrent(CurrentWeather&& current)
{
    weather_info_.setValue(std::move(current));
}

void WeatherCollector::SetForecast(ForecastWeather&& forecast)
{
     weather_info_.setValue(std::move(forecast));
}
CurrentWeather WeatherCollector::GetCurrentWeather() const
{
     return weather_info_.value<CurrentWeather>();
}
ForecastWeather WeatherCollector::GerForecastWeather() const
{
     return weather_info_.value<ForecastWeather>();
}

bool WeatherCollector::IsValid() const
{
     return weather_info_.isValid();
}
