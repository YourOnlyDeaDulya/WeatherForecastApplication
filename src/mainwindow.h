#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

#include "weatherdatacollector.h"
#include "currentwindow.h"
#include "forecastwindow.h"


//Подключиться к GitHub
//Декомпозиция + ренейминг. (По принципу ответственности - то есть "что делает")
//QSerializer
//При старте приложения фоново запускается проверка кэша на сохраненную дату последнего обращения (Обновляется при перезаписи)
//НАЧАЛИ ДЕЛАЦ
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showResultButtonClicked();
private slots:
    void on_comboBox_activated(int index);

private:

    enum class INFO_TYPE
    {
        CURRENT,
        FORECAST,
    };

    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<ResultWindow> results_;
    WeatherCollector collector_;
    QSqlDatabase db_;

    //Декомпозировать классы на интерфейсы и разные файлы + посмотреть Dependency Injection + Inversion of Control
    void HideDaysCountBar();
    void ShowDaysCountBar();
    bool CurrentWeatherRequest();
    bool ForecastWeatherRequest();
    QJsonDocument MakeRequest(INFO_TYPE type);

    bool CheckForInternetConnection();
    bool ConnectToDataBase();
    bool LoadLastRequest(INFO_TYPE type);
    void SaveCurrentToLocal(const CurrentWeather& current);
    bool LoadLastCurrentRequest();
    void SaveForecastToLocal(const ForecastWeather& forecast);
    bool LoadLastForecastRequest();
    void SaveForecastDays(const QVector<ForecastDay>& days);
    bool LoadForecastdays(ForecastWeather& forecast);

    const QString API_CODE = "acff18c1162c419a925183952230511";

};
#endif // MAINWINDOW_H
