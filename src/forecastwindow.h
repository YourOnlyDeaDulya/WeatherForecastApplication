#ifndef FORECASTWINDOW_H
#define FORECASTWINDOW_H

#include <QDialog>
#include "resultwindow.h"
#include "additionalnfowindow.h"
#include "weatherdatacollector.h"
#include "temperaturerecordswidget.h"
#include <memory>

namespace Ui {
class ForecastWindow;
}

class ForecastWindow : public ResultWindow
{
    Q_OBJECT

public:
    explicit ForecastWindow(QWidget *parent = nullptr);
    ~ForecastWindow();
    virtual void setAnswer(const WeatherCollector& w_collector) override;
public slots:
    virtual void backToMenuButtonClicked() override;
    void additionalInfoButtonClicked();
private:
    std::unique_ptr<Ui::ForecastWindow> ui;
    std::unique_ptr<TemperatureRecordsWidget> temperature_gistogramm_;
    std::unique_ptr<AdditionalnfoWindow>  additional_info_;
};

#endif // FORECASTWINDOW_H
