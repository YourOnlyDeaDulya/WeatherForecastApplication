#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QDialog>
#include "weatherdatacollector.h"

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultWindow(QWidget *parent = nullptr);
    virtual ~ResultWindow();
    virtual void setAnswer(const WeatherCollector& w_collector) = 0;
    static QFont CreateTextFont();
public slots:
    virtual void backToMenuButtonClicked() = 0;
};

#endif // RESULTWINDOW_H
