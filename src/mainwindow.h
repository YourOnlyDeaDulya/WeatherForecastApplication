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


//Подключиться к GitHub - СДЕЛАНО

//Декомпозиция + ренейминг. (По принципу ответственности - то есть "что делает")
//Cделал - в гите закомитить с сообщением "Такое-то изменение"
//QSerializer
//При старте приложения фоново запускается проверка кэша на сохраненную дату последнего обращения (Обновляется при перезаписи)

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
    //void on_daysCountInput_editingFinished();
private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<ResultWindow> results_;
    QSqlDatabase db_;
    std::unique_ptr<APIRequestView> request_handler_;
    std::unique_ptr<APIQueryView> query_constructor_;
    //Декомпозировать классы на интерфейсы и разные файлы + посмотреть Dependency Injection + Inversion of Control
    void HideDaysCountBar();
    void ShowDaysCountBar();

    bool MakeAPIRequest(INFO_TYPE type, WeatherCollector& w_collector);

    bool ConnectToDataBase(); //DB
    bool LoadLastRequest(INFO_TYPE type); //DB
    void SaveCurrentToLocal(const CurrentWeather& current); //DB
    bool LoadLastCurrentRequest(); //DB
    void SaveForecastToLocal(const ForecastWeather& forecast); //DB
    bool LoadLastForecastRequest(); //DB
    void SaveForecastDays(const QVector<ForecastDay>& days); //DB
    bool LoadForecastdays(ForecastWeather& forecast); //DB

    const QMap<INFO_TYPE, std::unique_ptr<ResultWindow>> request_type_to_window_
    {
        {INFO_TYPE::CURRENT, std::make_unique<CurrentWindow>(this)},
        {INFO_TYPE::FORECAST, std::make_unique<ForecastWindow>(this)},
    };


};

#endif // MAINWINDOW_H
