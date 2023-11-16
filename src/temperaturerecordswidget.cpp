// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "temperaturerecordswidget.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QLegend>
#include <QStackedBarSeries>
#include <QValueAxis>
#include "resultwindow.h"

QString GetDayPronounce(int count)
{
    if(count == 1)
    {
        return " день";
    }
    else if(count > 1 && count < 5)
    {
        return " дня";
    }
    else
    {
        return " дней";
    }
}

TemperatureRecordsWidget::TemperatureRecordsWidget(QWidget *parent, const QVector<ForecastDay>& forecast_days)
    : ContentWidget(parent)
{

    auto *high = new QBarSet("Температура в градусах цельсия");

    for(const auto& day : forecast_days)
    {
        *high << day.avg_t;
    }
    //![1]

    //![2]
    auto *series = new QStackedBarSeries();
    series->append(high);
    //![2]

    //![3]
    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Погода на " + QString::number(forecast_days.size()) + GetDayPronounce(forecast_days.size()));
    chart->setTitleFont(ResultWindow::CreateTextFont());
    chart->setAnimationOptions(QChart::SeriesAnimations);
    //![3]

    //![4]
    QStringList categories;
    categories.reserve(forecast_days.size());
    for(const auto& day : forecast_days)
    {
        QString date = QString::number(day.date_.day_) + "-" + QString::number(day.date_.month_)
                       + "-" + QString::number(day.date_.year_ - 2000);
        date += ", " + QString::number(day.avg_t) + "C";
        categories.push_back(std::move(date));
    }

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Дата, [C]");
    QFont f;
    f.setBold(true);
    axisX->setTitleFont(f);
    chart->addAxis(axisX, Qt::AlignBottom);

    auto *axisY = new QValueAxis();
    axisY->setRange(-35, 35);
    axisY->setTitleText("Температура [&deg;C]");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    //![4]

    //![5]
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    //![5]

    //![6]
    createDefaultChartView(chart);
    //![6]
}
