#ifndef APIQUERYCONSTRUCTOR_H
#define APIQUERYCONSTRUCTOR_H

#include <QString>
#include "errormessage.h"
#include <optional>
#include <QMap>

enum INFO_TYPE
{
    CURRENT,
    FORECAST,
};

struct RequestDto
{
    std::optional<QString> city_;
    std::optional<QString> days_count_;
    INFO_TYPE type_;
};

class APIQueryConstructorInterface //переименовать
{
public:
    virtual QString GetQuery(INFO_TYPE request_type, const RequestDto& request) const = 0;
    virtual ~APIQueryConstructorInterface() = default;
};

class APIQueryConstructor final : public APIQueryConstructorInterface
{
private:

    const QString API_CODE_ = "26f8dedf8a8549e395894859232111";
    const QString END_STATEMENT_ = "&aqi=no&lang=ru"; //Пока что обязательный конец запроса.
                                                      //Можно модифицировать, подобно обязательным методам,
                                                      //Чтобы передавать список запросов на добавление.

    static QString GetMainQuery(const RequestDto& request);
    static QString GetDaysQuery(const RequestDto& request);

    inline static const QMap<INFO_TYPE, QVector<std::function<QString(const RequestDto&)>>> mandatory_methods_list_//Починить
        {
         {INFO_TYPE::CURRENT, {&GetMainQuery}},
         {INFO_TYPE::FORECAST, {&GetMainQuery, &GetDaysQuery}},
         };

    const QMap<INFO_TYPE, QString> request_opening_
        {
         { INFO_TYPE::CURRENT, "http://api.weatherapi.com/v1/current.json?key=" + API_CODE_ },
         { INFO_TYPE::FORECAST, "http://api.weatherapi.com/v1/forecast.json?key=" + API_CODE_ },
         };

public:
    virtual QString GetQuery(INFO_TYPE request_type, const RequestDto& request) const override;
};

#endif // APIQUERYCONSTRUCTOR_H
