#ifndef CURRENTWINDOW_H
#define CURRENTWINDOW_H

#include <QDialog>
#include <memory>
#include "weatherdatacollector.h"
#include "resultwindow.h"
#include <QMainWindow>

namespace Ui {
class CurrentWindow;
}

class CurrentWindow : public ResultWindow
{
    Q_OBJECT
public:
    explicit CurrentWindow(QWidget *parent = nullptr);
    ~CurrentWindow();
    void setAnswer(const WeatherCollector& w_collector) override;
public slots:
    void backToMenuButtonClicked() override;
private:
    std::unique_ptr<Ui::CurrentWindow> ui;
};

#endif // CURRENTWINDOW_H
