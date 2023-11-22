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
#include <QMap>


#include "weatherdatacollector.h"
#include "currentwindow.h"
#include "forecastwindow.h"
#include "errormessage.h"
#include "apirequesthandler.h"
#include "request_info_types.h"
#include "sqlcachingservice.h"


//Подключиться к GitHub - СДЕЛАНО

//Декомпозиция + ренейминг. (По принципу ответственности - то есть "что делает")
//Cделал - в гите закомитить с сообщением "Такое-то изменение"
//QSerializer
//При старте приложения фоново запускается проверка кэша на сохраненную дату последнего обращения (Обновляется при перезаписи)


//доделать API, переименовать классы, переделать базу данных с добавлением даты запроса и сделать класс-очиститель базы данных.
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
    void daysCountInput_edited(const QString &arg1);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<ResultWindow> results_;

    std::unique_ptr<CachingServiceInterface> caching_service_;
    std::unique_ptr<APIForecastServiceInterface> api_forecast_service_;

    void HideDaysCountBar();
    void ShowDaysCountBar();

    bool TryMakeAPIRequest(INFO_TYPE type, WeatherCollector& w_collector);


    const QMap<INFO_TYPE, std::function<ResultWindow*(QWidget*)>> request_type_to_window_
    {
        {INFO_TYPE::CURRENT, [](QWidget* parent) { return new CurrentWindow(parent); }},
        {INFO_TYPE::FORECAST, [](QWidget* parent) { return new ForecastWindow(parent); }},
    };
};

#endif // MAINWINDOW_H
