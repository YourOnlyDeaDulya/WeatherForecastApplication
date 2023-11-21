#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , request_handler_(std::make_unique<APIForecastService>(this))
{
    ui->setupUi(this);
    QObject::connect(ui->showResultButton, &QPushButton::clicked,
                     this, &MainWindow::showResultButtonClicked);
    QObject::connect(ui->daysCountInput, &QLineEdit::textEdited,
                     this, &MainWindow::daysCountInput_edited);
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
        return;
    }

    HideDaysCountBar();
}

void MainWindow::showResultButtonClicked()
{
    bool request_succeeded = true;
    INFO_TYPE request_type = static_cast<INFO_TYPE>(ui->comboBox->currentIndex());

    results_.reset(request_type_to_window_[request_type](this));

    if(!request_handler_->CheckForInternetConnection(this))
    {
        if(!LoadLastRequest(request_type))
        {
            results_.reset();
            return;
        }

        this->hide();
        results_->exec();
        return;
    }

    WeatherCollector w_collector;
    if(!TryMakeAPIRequest(request_type, w_collector))
    {
        results_.reset();
        return;
    };

    results_->setAnswer(std::move(w_collector));

    this->hide();
    results_->exec();
}

void MainWindow::daysCountInput_edited(const QString& text)
{
    if(text.toInt() < 1 || text.toInt() > 10)
    {
        ErrorMessage("Введите корректное число дней");
        ui->daysCountInput->clear();
    }
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

bool MainWindow::TryMakeAPIRequest(INFO_TYPE type, WeatherCollector& w_collector)
{
    RequestDto request_dto;
    request_dto.type_ = type;
    switch(type)
    {
    case INFO_TYPE::CURRENT:
        request_dto.city_ = ui->cityLineEdit->text();
        break;
    case INFO_TYPE::FORECAST:
        request_dto.city_ = ui->cityLineEdit->text();
        request_dto.days_count_ = ui->daysCountInput->text();
        break;
    }

    w_collector = request_handler_->GetRequestAnswer(std::move(request_dto), type);

    if(!w_collector.IsValid())
    {
        return false;
    }

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
        LoadLastCurrentRequest();
        break;

    case INFO_TYPE::FORECAST:
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
    WeatherCollector w_collector;

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

            w_collector.SetCurrent(std::move(current));
        }

        results_->setAnswer(w_collector);
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
    WeatherCollector w_collector;
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

            w_collector.SetForecast(std::move(forecast));
        }

        results_->setAnswer(w_collector);
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
