#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , caching_service_(std::make_unique<CachingService>())
    , api_forecast_service_(std::make_unique<APIForecastService>(this))
{
    ui->setupUi(this);
    QObject::connect(ui->showResultButton, &QPushButton::clicked,
                     this, &MainWindow::showResultButtonClicked);
    QObject::connect(ui->daysCountInput, &QLineEdit::textEdited,
                     this, &MainWindow::daysCountInput_edited);
    HideDaysCountBar();
    this->setWindowTitle("Прогноз погоды");
    caching_service_->TryCleanCache();
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
    INFO_TYPE request_type = static_cast<INFO_TYPE>(ui->comboBox->currentIndex());
    WeatherCollector w_collector;

    results_.reset(request_type_to_window_[request_type](this));

    switch(api_forecast_service_->CheckForInternetConnection(this))
    {
    case true:
        if(!TryMakeAPIRequest(request_type, w_collector))
        {
            results_.reset();
            return;
        }

        caching_service_->TryCacheLastRequest(request_type, w_collector);
        break;

    case false:
        if(!caching_service_->TryLoadLastRequest(request_type, w_collector, ui->cityLineEdit->text()))
        {
            results_.reset();
            return;
        }
        break;
    }

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

    w_collector = api_forecast_service_->GetRequestAnswer(std::move(request_dto), type);

    if(!w_collector.IsValid())
    {
        return false;
    }

    return true;
}
