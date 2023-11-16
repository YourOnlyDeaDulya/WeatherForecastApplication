#include "current_window.h"
#include "ui_current_window.h"

CurrentWeatherDialog::CurrentWeatherDialog(QWidget *parent) :
    QDialog(parent),
    ui(std::make_unique<Ui::Dialog>())
{
    ui->setupUi(this);
    ui->cityCountryLabel->setFont(CreateTextFont());
    QObject::connect(ui->backToMenuButton, &QPushButton::clicked,
                     this, &CurrentWeatherDialog::backToMenuButtonClicked);
}

CurrentWeatherDialog::~CurrentWeatherDialog()
{}

QFont CurrentWeatherDialog::CreateTextFont()
{
    QFont font;
    font.setPixelSize(30);

    return font;
}

void CurrentWeatherDialog::setResult(const WeatherCollector& w_collector)
{
    const CurrentWeather& current = w_collector.GetCurrentWeather();
    ui->cityCountryLabel->setText(current.city_ + ", " + current.country_);

    QString date = QString::number(current.date_.year_) + "-" + QString::number(current.date_.month_)
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

#include "mainwindow.h"

void CurrentWeatherDialog::backToMenuButtonClicked()
{
    this->hide();
    qobject_cast<MainWindow*>(parent())->show();
}
