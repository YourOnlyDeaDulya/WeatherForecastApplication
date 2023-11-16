#include "currentwindow.h"
#include "ui_currentwindow.h"
#include "mainwindow.h"

CurrentWindow::CurrentWindow(QWidget *parent) :
    ResultWindow(parent),
    ui(std::make_unique<Ui::CurrentWindow>())
{
    ui->setupUi(this);
    this->setWindowTitle("Температура сейчас");
    QObject::connect(ui->backToMenuButton, &QPushButton::clicked,
                     this, &CurrentWindow::backToMenuButtonClicked);
}

CurrentWindow::~CurrentWindow()
{
}

void CurrentWindow::setAnswer(const WeatherCollector& w_collector)
{
    CurrentWeather current = w_collector.GetCurrentWeather();
    ui->cityCountryLabel->setText(current.city_ + ", " + current.country_);
    ui->cityCountryLabel->setFont(CreateTextFont());
    QString date = QString::number(current.date_.year_)
                   + "-" + QString::number(current.date_.month_)
                   + "-" + QString::number(current.date_.day_);
    ui->dateResultLabel->setText(date);

    QString zero = current.hourly_weather_info_.time_.minute_ < 10 ? "0" : "";
    QString time = QString::number(current.hourly_weather_info_.time_.hour_) + ":" + zero + QString::number(current.hourly_weather_info_.time_.minute_);
    ui->timeResultLabel->setText(time);

    ui->tempResultLabel->setText(QString::number(current.hourly_weather_info_.celcium_t_));
    ui->feelsLikeResulLabel->setText(QString::number(current.hourly_weather_info_.feels_like_t_));

    QString is_day = current.hourly_weather_info_.is_day_ ? "День" : "Ночь";
    ui->weatherResultLabel->setText(is_day + ",\n" + current.hourly_weather_info_.weather_condition_);
}

void CurrentWindow::backToMenuButtonClicked()
{
    qobject_cast<MainWindow*>(parent())->show();
    this->close();
}
