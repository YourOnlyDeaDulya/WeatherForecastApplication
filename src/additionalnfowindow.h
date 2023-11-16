#ifndef ADDITIONALNFOWINDOW_H
#define ADDITIONALNFOWINDOW_H

#include <QDialog>
#include <memory>
#include "weatherdatacollector.h"
#include "resultwindow.h"

namespace Ui {
class AdditionalnfoWindow;
}

class AdditionalnfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdditionalnfoWindow(QWidget *parent = nullptr);
    ~AdditionalnfoWindow();
    void MakeInfoTable(const ForecastWeather& forecast);
public slots:
    void closeButtonClicked();
private:
    std::unique_ptr<Ui::AdditionalnfoWindow> ui;
};

#endif // ADDITIONALNFOWINDOW_H
