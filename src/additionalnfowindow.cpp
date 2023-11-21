#include "additionalnfowindow.h"
#include "ui_additionalnfowindow.h"
#include "resultwindow.h"

AdditionalnfoWindow::AdditionalnfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(std::make_unique<Ui::AdditionalnfoWindow>())
{
    ui->setupUi(this);
    this->setWindowTitle("Подробная информация");
    QObject::connect(ui->closeButton, &QPushButton::clicked,
                     this, &AdditionalnfoWindow::closeButtonClicked);

}

AdditionalnfoWindow::~AdditionalnfoWindow()
{
}

void AdditionalnfoWindow::MakeInfoTable(const ForecastWeather& forecast)
{
    ui->cityCountryLabel->setText(forecast.city_ + ", " + forecast.country_);
    ui->cityCountryLabel->setFont(ResultWindow::CreateTextFont());

    ui->weatherInfoTable->setRowCount(4);
    ui->weatherInfoTable->setColumnCount(forecast.forecast_days_.size());

    QStringList dates;
    dates.reserve(forecast.forecast_days_.size());
    for(const auto& day : forecast.forecast_days_)
    {
        dates.emplace_back(QString::number(day.date_.day_)
                           + "-" + QString::number(day.date_.month_)
                           + "-" + QString::number(day.date_.year_));
    }

    ui->weatherInfoTable->setHorizontalHeaderLabels(dates);
    ui->weatherInfoTable->setVerticalHeaderLabels(QStringList() << "Минимальная температура"
                                                                << "Максимальная температура"
                                                                << "Cредняя температура"
                                                                << "Погода");

    for(int i = 0; i < ui->weatherInfoTable->columnCount(); ++i)
    {
        const auto& day = forecast.forecast_days_[i];

        QTableWidgetItem *min_t = new QTableWidgetItem(QString::number(day.min_max_t_.first));
        min_t->setFlags(min_t->flags() & ~Qt::ItemIsEditable);
        ui->weatherInfoTable->setItem(0, i, min_t);

        QTableWidgetItem *max_t = new QTableWidgetItem(QString::number(day.min_max_t_.second));
        max_t->setFlags(max_t->flags() & ~Qt::ItemIsEditable);
        ui->weatherInfoTable->setItem(1, i, max_t);

        QTableWidgetItem *avg_t = new QTableWidgetItem(QString::number(day.avg_t));
        avg_t->setFlags(avg_t->flags() & ~Qt::ItemIsEditable);
        ui->weatherInfoTable->setItem(2, i, avg_t);

        QTableWidgetItem *w_condition = new QTableWidgetItem(day.weather_condition_);
        w_condition->setFlags(w_condition->flags() & ~Qt::ItemIsEditable);
        ui->weatherInfoTable->setItem(3, i, w_condition);
    }

    ui->weatherInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AdditionalnfoWindow::closeButtonClicked()
{
    this->hide();
}

