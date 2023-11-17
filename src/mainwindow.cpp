#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , request_handler_(std::make_unique<APIRequestModel>(this))
    , query_constructor_(std::make_unique<APIQueryModel>())
{
    ui->setupUi(this);
    QObject::connect(ui->showResultButton, &QPushButton::clicked,
                     this, &MainWindow::showResultButtonClicked);
    HideDaysCountBar();
    this->setWindowTitle("Прогноз погоды");
    ConnectToDataBase();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_comboBox_activated(int index)
{
    if(index == 1)
    {
        ShowDaysCountBar();
    }
    else
    {
        HideDaysCountBar();
    }
}

void MainWindow::showResultButtonClicked()
{
    if (results_)
    {
        results_.reset();
    }

    bool request_succeeded = true;
    if(!request_handler_->CheckForInternetConnection(this))
    {
        request_succeeded = LoadLastRequest(ui->comboBox->currentIndex() == 0 ? //static_cast<INFO_TYPE>
                            INFO_TYPE::CURRENT : INFO_TYPE::FORECAST);
        return;
    }//вместо else - return внутри if

    std::unique_ptr<WeatherCollector> w_collector = nullptr;
    switch(ui->comboBox->currentIndex())
    {
    case 0:

        results_ = std::make_unique<CurrentWindow>(this);
        request_succeeded = CurrentWeatherRequest();
        break;

    case 1:

        results_ = std::make_unique<ForecastWindow>(this);
        request_succeeded = ForecastWeatherRequest();
        break;
    }

    if(!request_succeeded)
    {
        results_.reset();
        return;
    }

    this->hide();
    results_->exec();
}

void MainWindow::HideDaysCountBar()
{
    ui->daysCountInput->hide();
    ui->daysCountLabel->hide();
}

void MainWindow::ShowDaysCountBar()
{
    ui->daysCountInput->show();
    ui->daysCountLabel->show();
}


bool MainWindow::MakeAPIRequest(INFO_TYPE type, WeatherCollector& w_collector)
{
    switch(type)
    {
    case INFO_TYPE::CURRENT:
        query_constructor_->SetMainQuery(ui->cityLineEdit->text());
        break;
    case INFO_TYPE::FORECAST:
        query_constructor_->SetMainQuery(ui->cityLineEdit->text());
        query_constructor_->SetDaysQuery(ui->daysCountInput->text());
        break;
    }

    w_collector = request_handler_->GetRequestAnswer(query_constructor_->GetQuery(type), type);

    if(!w_collector.IsValid())
    {
        return false;
    }

    return true;
}

CurrentWeather MakeCurrentWeather(const QVariantMap& map)
{
    const QVariantMap& location = map["location"].toJsonObject().toVariantMap();
    const QVariantMap& current = map["current"].toJsonObject().toVariantMap();

    CurrentWeather current_weather;
    current_weather.city_ = location["name"].toString();
    current_weather.country_ = location["country"].toString();

    QPair<Date, Time> dt = ParseDateTimeFromString(location["localtime"].toString());
    current_weather.date_ = dt.first;
    current_weather.hourly_weather_info_.time_ = dt.second;

    current_weather.hourly_weather_info_.celcium_t_ = current["temp_c"].toDouble();
    current_weather.hourly_weather_info_.feels_like_t_ = current["feelslike_c"].toDouble();
    current_weather.hourly_weather_info_.is_day_ = current["is_day"].toBool();
    current_weather.hourly_weather_info_.weather_condition_ = current["condition"].toJsonObject().toVariantMap()["text"].toString();

    return current_weather;
}

ForecastWeather MakeForecastWeather(const QVariantMap& map)
{
    const QVariantMap& location = map["location"].toJsonObject().toVariantMap();
    const QVariantMap& forecast = map["forecast"].toJsonObject().toVariantMap();

    ForecastWeather forecast_weather;
    forecast_weather.city_ = location["name"].toString();
    forecast_weather.country_ = location["country"].toString();

    const auto& forecast_days = forecast["forecastday"].toJsonArray();
    for(const auto& json_day : forecast_days)
    {
        ForecastDay forecast_day;
        const auto& day_map = json_day.toObject().toVariantMap();

        forecast_day.date_ = ParseDateFromString(day_map["date"].toString());

        const auto& day_info = day_map["day"].toJsonObject().toVariantMap();

        forecast_day.avg_t = day_info["avgtemp_c"].toDouble();

        double min_t = day_info["mintemp_c"].toDouble();
        double max_t = day_info["maxtemp_c"].toDouble();
        forecast_day.min_max_t_ = {min_t, max_t};

        forecast_day.weather_condition_ = day_info["condition"].toJsonObject().toVariantMap()["text"].toString();

        forecast_weather.forecast_days_.push_back(std::move(forecast_day));
    }

    return forecast_weather;
}

bool MainWindow::CurrentWeatherRequest()
{
    QVariantMap map = MakeRequest(INFO_TYPE::CURRENT).object().toVariantMap();

    if(map.contains("error"))
    {
        ErrorMessage(map["error"].toJsonObject().toVariantMap()["message"].toString());
        return false;
    }

    CurrentWeather current = MakeCurrentWeather(map); //Заняться ренеймингом.
    SaveCurrentToLocal(current); //Асинхронное + mutex.
    collector_.SetCurrent(std::move(current));
    results_->setAnswer(collector_);

    return true;
}

bool MainWindow::ForecastWeatherRequest()
{
    if(ui->daysCountInput->text().toInt() < 1 || ui->daysCountInput->text().toInt() > 10)
    {
        ErrorMessage("Введите корректное число дней");
        return false;
    }

    QVariantMap map = MakeRequest(INFO_TYPE::FORECAST).object().toVariantMap();

    if(map.contains("error"))
    {
        ErrorMessage(map["error"].toJsonObject().toVariantMap()["message"].toString());
        return false;
    }

    ForecastWeather forecast = MakeForecastWeather(map);
    SaveForecastToLocal(forecast);
    collector_.SetForecast(std::move(forecast));
    results_->setAnswer(collector_);

    return true;
}

QJsonDocument MainWindow::MakeRequest(INFO_TYPE type)
{
    std::unique_ptr<QNetworkAccessManager> mNetManager = std::make_unique<QNetworkAccessManager>(this);
    QString url_s = type == INFO_TYPE::CURRENT ? //Словарь с АПИ?
                        "http://api.weatherapi.com/v1/current.json?key=" + API_CODE + "&q=" + ui->cityLineEdit->text() + "&aqi=no&lang=ru"
                                               :
                        "http://api.weatherapi.com/v1/forecast.json?key=" + API_CODE + "&q="
                            + ui->cityLineEdit->text() + "&days=" + ui->daysCountInput->text() + "&aqi=no&lang=ru";
    QUrl url = QUrl(url_s);
    QNetworkRequest req(url);
    std::unique_ptr<QNetworkReply> reply;
    reply.reset(mNetManager->get(req));
    QEventLoop loop;
    QObject::connect(reply.get(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString jsonString = reply->readAll();
    return QJsonDocument::fromJson(jsonString.toUtf8());
}

bool MainWindow::CheckForInternetConnection()
{
    std::unique_ptr<QTcpSocket> sock = std::make_unique<QTcpSocket>(this);
    sock->connectToHost("www.google.com", 80);
    bool connected = sock->waitForConnected(30000);

    if (!connected)
    {
        sock->abort();
        ErrorMessage("Отсутствует интернет-соединение. Будет показан последний запрос");
        return false;
    }
    sock->close();
    return true;
}

bool MainWindow::ConnectToDataBase()
{
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("./../../sqlite_db/weather_request.db");
    return db_.open();
}

bool MainWindow::LoadLastRequest(INFO_TYPE type) //Словарь запрос/метод
{
    if(!db_.isOpen())
    {
        ErrorMessage("Отсутствует подключение к локальной базе данных");
        return false;
    }

    switch(type)
    {
    case INFO_TYPE::CURRENT:
        results_ = std::make_unique<CurrentWindow>(this);
        LoadLastCurrentRequest();
        break;

    case INFO_TYPE::FORECAST:
        results_ = std::make_unique<ForecastWindow>(this);
        LoadLastForecastRequest();
        break;
    }

    return true;
}

void MainWindow::SaveCurrentToLocal(const CurrentWeather& current)
{
    QSqlQuery query;
    QString query_text = "UPDATE CurrentWeather SET ";
    query_text
    += "city = '" + current.city_ + "', "
    + "country = '" + current.country_ + "', "
    + "day = " + QString::number(current.date_.day_) + ", "
    + "month = " + QString::number(current.date_.month_) + ", "
    + "year = " + QString::number(current.date_.year_) + ", "
    + "hour = " + QString::number(current.hourly_weather_info_.time_.hour_) + ", "
    + "minute = " + QString::number(current.hourly_weather_info_.time_.minute_) + ", "
    + "weather_condition = '" + current.hourly_weather_info_.weather_condition_ + "', "
    + "celcium_t = " + QString::number(current.hourly_weather_info_.celcium_t_) + ", "
    + "feels_like = " + QString::number(current.hourly_weather_info_.feels_like_t_) + ", "
    + "is_day = " + QString::number(current.hourly_weather_info_.is_day_);

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
    }

}

bool MainWindow::LoadLastCurrentRequest()
{
    QSqlQuery query;
    QString query_text = "SELECT city, country, day, month, year, hour, minute, "
                         "weather_condition, celcium_t, feels_like, is_day "
                         "FROM CurrentWeather";
    if(query.exec(std::move(query_text)))
    {
        while(query.next())
        {
            CurrentWeather current;

            current.city_ = query.value(0).toString();
            current.country_ = query.value(1).toString();

            current.date_.day_ = query.value(2).toInt();
            current.date_.month_ = query.value(3).toInt();
            current.date_.year_ = query.value(4).toInt();

            current.hourly_weather_info_.time_.hour_ =
                                query.value(5).toInt();
            current.hourly_weather_info_.time_.minute_ =
                                query.value(6).toInt();

            current.hourly_weather_info_.weather_condition_ =
                                query.value(7).toString();
            current.hourly_weather_info_.celcium_t_ =
                                query.value(8).toDouble();
            current.hourly_weather_info_.feels_like_t_ =
                                query.value(9).toDouble();
            current.hourly_weather_info_.is_day_ =
                                query.value(10).toBool();

            collector_.SetCurrent(std::move(current));
        }

        results_->setAnswer(collector_);
        return true;
    }
    else
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        return false;
    }
}

void MainWindow::SaveForecastToLocal(const ForecastWeather& forecast)
{
    QSqlQuery query;
    QString query_text = "UPDATE ForecastWeather SET city = '" + forecast.city_ + "', "
                                                    "country = '" + forecast.country_ + "'";

    if(!query.exec(std::move(query_text)))
    {
        ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
        return;
    }

    SaveForecastDays(forecast.forecast_days_);
}

bool MainWindow::LoadLastForecastRequest()
{
    QSqlQuery query;
    QString query_text = "SELECT city, country FROM ForecastWeather";

    if(query.exec(std::move(query_text)))
    {
        while(query.next())
        {
            ForecastWeather forecast;

            forecast.city_ = query.value(0).toString();
            forecast.country_ = query.value(1).toString();

            if(!LoadForecastdays(forecast))
            {
                ErrorMessage("Не удалось загрузить forecast_days: " + query.lastError().text());
                return false;
            }

            collector_.SetForecast(std::move(forecast));
        }

        results_->setAnswer(collector_);
        return true;
    }
    else
    {
        ErrorMessage("Не удалось загрузить последний запрос: " + query.lastError().text());
        return false;
    }
}

void MainWindow::SaveForecastDays(const QVector<ForecastDay>& days)
{
    QSqlQuery query;
    if(!query.exec("DELETE FROM ForecastDays"))
    {
        ErrorMessage("Не удалось очистить кэш: " + query.lastError().text());
        return;
    }

    for(const auto& day : days)
    {
        QString query_text = "INSERT INTO ForecastDays (avg_t, min_t, max_t, "
                             "weather_condition, day, month, year) VALUES (";
        query_text
        += QString::number(day.avg_t) + ", "
        + QString::number(day.min_max_t_.first) + ", "
        + QString::number(day.min_max_t_.second) + ", '"
        + day.weather_condition_ + "', "
        + QString::number(day.date_.day_) + ", "
        + QString::number(day.date_.month_) + ", "
        + QString::number(day.date_.year_) + ")";

        if(!query.exec(std::move(query_text)))
        {
            ErrorMessage("Не удалось кэшировать запрос: " + query.lastError().text());
            return;
        }
    }
}

bool MainWindow::LoadForecastdays(ForecastWeather& forecast)
{
    QSqlQuery query;
    QString query_text = "SELECT avg_t, min_t, max_t, "
                         "weather_condition, day, month, year "
                         "FROM ForecastDays";

    if(query.exec(std::move(query_text)))
    {
        while(query.next())
        {
            ForecastDay day;

            day.avg_t = query.value(0).toDouble();
            day.min_max_t_.first = query.value(1).toDouble();
            day.min_max_t_.second = query.value(2).toDouble();

            day.weather_condition_ = query.value(3).toString();

            day.date_.day_ = query.value(4).toInt();
            day.date_.month_ = query.value(5).toInt();
            day.date_.year_ = query.value(6).toInt();

            forecast.forecast_days_.push_back(std::move(day));
        }

        return true;
    }

    return false;
}
