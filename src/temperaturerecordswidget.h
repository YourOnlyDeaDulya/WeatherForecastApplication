#ifndef TEMPERATURERECORDSWIDGET_H
#define TEMPERATURERECORDSWIDGET_H

#include "contentwidget.h"
#include "weatherdatacollector.h"

class TemperatureRecordsWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit TemperatureRecordsWidget(QWidget *parent, const QVector<ForecastDay>& forecast_days);
};

#endif
