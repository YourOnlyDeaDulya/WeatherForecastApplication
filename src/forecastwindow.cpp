#include "forecastwindow.h"
#include "ui_forecastwindow.h"
#include "mainwindow.h"

ForecastWindow::ForecastWindow(QWidget *parent) :
    ResultWindow(parent),
    ui(std::make_unique<Ui::ForecastWindow>())
{
    ui->setupUi(this);
    this->setWindowTitle("Прогноз на период времени");
    QObject::connect(ui->backToMenuButton, &QPushButton::clicked,
                     this, &ForecastWindow::backToMenuButtonClicked);
    QObject::connect(ui->additionalInfoButton, &QPushButton::clicked,
                     this, &ForecastWindow::additionalInfoButtonClicked);
}

ForecastWindow::~ForecastWindow()
{
}

void ForecastWindow::setAnswer(const WeatherCollector& w_collector)
{
    ForecastWeather forecast = w_collector.GerForecastWeather();
    temperature_gistogramm_ = std::make_unique<TemperatureRecordsWidget>(this, forecast.forecast_days_);
    temperature_gistogramm_->load();
    temperature_gistogramm_->resize(ui->frame->size());
    temperature_gistogramm_->setVisible(true);

    additional_info_ = std::make_unique<AdditionalnfoWindow>(this);
    additional_info_->MakeInfoTable(forecast);
}

void ForecastWindow::backToMenuButtonClicked()
{
    qobject_cast<MainWindow*>(parent())->show();
    this->close();
}

void ForecastWindow::additionalInfoButtonClicked()
{
    additional_info_->show();
}
