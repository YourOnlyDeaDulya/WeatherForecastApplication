#pragma once

#ifndef WEATHERDATACOLLECTOR_H
#define WEATHERDATACOLLECTOR_H

#include <QString>
#include <QRegularExpression>
#include <QVariant>


struct Date
{
    int day_;
    int month_;
    int year_;
};

struct Time
{
    int hour_;
    int minute_;
};

QPair<Date, Time> ParseDateTimeFromString(const QString& str);
Date ParseDateFromString(const QString& str);

struct HourlyWeather
{
    HourlyWeather() = default;
    HourlyWeather(const HourlyWeather& other);
    HourlyWeather(HourlyWeather&& other);
    HourlyWeather& operator=(const HourlyWeather& other);
    HourlyWeather& operator=(HourlyWeather&& other);
    Time time_;
    QString weather_condition_;
    double celcium_t_;
    double feels_like_t_;
    bool is_day_;
};

struct CurrentWeather
{
    CurrentWeather() = default;
    CurrentWeather(const CurrentWeather& other);
    CurrentWeather(CurrentWeather&& other);
    CurrentWeather& operator=(const CurrentWeather& other);
    CurrentWeather& operator=(CurrentWeather&& other);
    QString city_;
    QString country_;
    Date date_;
    HourlyWeather hourly_weather_info_;
};

struct ForecastDay
{
    ForecastDay() = default;
    ForecastDay(const ForecastDay& other);
    ForecastDay(ForecastDay&& other);
    ForecastDay& operator=(const ForecastDay& other);
    ForecastDay& operator=(ForecastDay&& other);
    QPair<double, double> min_max_t_;
    double avg_t;
    QString weather_condition_;
    Date date_;
};

struct ForecastWeather
{
    ForecastWeather() = default;
    ForecastWeather(const ForecastWeather& other);
    ForecastWeather(ForecastWeather&& other);
    ForecastWeather& operator=(const ForecastWeather& other);
    ForecastWeather& operator=(ForecastWeather&& other);
    QString city_;
    QString country_;
    QVector<ForecastDay> forecast_days_;
};

class WeatherCollector
{
public:
    void SetCurrent(CurrentWeather&& curr);
    void SetForecast(ForecastWeather&& forect);
    CurrentWeather GetCurrentWeather() const;
    ForecastWeather GerForecastWeather() const;
private:
    QVariant weather_info_;
};



#endif // WEATHERDATACOLLECTOR_H
