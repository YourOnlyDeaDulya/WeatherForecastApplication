#include "apiqueryconstructor.h"


QString APIQueryConstructor::GetMainQuery(const RequestDto& request)
{
    return "&q=" + request.city_.value();
}

QString APIQueryConstructor::GetDaysQuery(const RequestDto& request)
{
    return "&days=" + request.days_count_.value();
}

QString APIQueryConstructor::GetQuery(INFO_TYPE request_type, const RequestDto& request) const
{
    QString result_query = request_opening_.value(request_type);

    const auto& methods_list = mandatory_methods_list_.value(request_type);
    for(qsizetype i = 0; i < methods_list.size(); ++i)
    {
        result_query += methods_list[i](request);
    }

    result_query += END_STATEMENT_;

    return result_query;
}
