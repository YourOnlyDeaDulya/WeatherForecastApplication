#pragma once

#ifndef RESULT_WINDOW_H
#define RESULT_WINDOW_H

#include <QDialog>
#include <memory>
#include "weatherdatacollector.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Dialog;
}
QT_END_NAMESPACE

class CurrentWeatherDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CurrentWeatherDialog(QWidget *parent = nullptr);
    ~CurrentWeatherDialog();
    void setResult(const WeatherCollector& w_collector);
    QFont CreateTextFont();
public slots:
    void backToMenuButtonClicked();
private:
    std::unique_ptr<Ui::Dialog> ui;
};

#endif// RESULT_WINDOW_H
