Приложение для просмотра прогноза погоды.
Позволяет посмотреть прогноз на несколько дней вперед или прогноз в текущее время.
Прогноз в текущее время показывается списком, на несколько дней - анимированной гистограммой и таблицей во вкладке "Подробнее"

При отсутствии интернета выводит локально сохраненный кэш. Если на нужный город есть информация - выводит нужный нам город, в 
другом случае - первый из локальных запросов. Кэш проверяется на валидность при каждом запуске программы, если дата запроса на кэш устарела (хотя бы на день) -
этот кэш очищается. При повторном вводе уже имеющегося города, кэш на этот город перезаписывается.

В окне "Город" нужно ввести город, в котором необходимо посмотреть погоду. 
Пока что работает только на один из городов с одинаковыми названиями (Петербург в США и Петербург в России, приоритет идет на США - от API)
В колонке "Количество дней" нужно ввести количество дней для прогноза, но не более 10.

Этот проект - первое знакомство с RestAPI, Qt и SQL, конкретнее - cервис https://www.weatherapi.com, Qt6.6.0 и SQLite.3.35.5. Проект удобен для расширения и модификаций, 
как в рамках предоставленного API, так и в рамках других API. (Если при работе с данным приложением будет выводить максимально 3 дня для прогноза на несколько дней,
то это значит, что закончился пробный доступ и АПИ перешла на бесплатный пакет. К сожалению, была возможность использовать только бесплатные RestAPI)

Инструкция по использованию:
1) Установка QtCreator, а также - Qt6, MinGW64 и OpenSSL 3.0.11 19 Sep 2023 (Library: OpenSSL 3.0.11 19 Sep 2023).
Особенно важен OpenSSL, так как при несовместимой версии с Qt6 HTTP запросы не будут корректно работать.
2) Настройка директорий:
*Папка для проекта*
  - bin         //Сюда будет собираться проект. Путь для сборки есть в файле .pro.user, но при ошибках подключения к бд - путь до базы данных в программе указан как "./../sqlite_db/cache.db". Можно ориентироваться на него.
  - sqlite_db   //Сюда будет помещен SQL-файл базы данных
  - src         //Папка для .h/.cpp/.pro файлов
3) Создание базы данных:
- Зайти в папку sqlite_db
- Открыть ее с помощью PowerShell или другой командной строки
- Применить команды:
  - sqlite3 cache.db
  - .read cache.db.sql
  - .exit (для выхода из SQLite)
(Для внешней работы с базами данных и ее проверки был использован DB Browser For SQLite 3.12.2, возможно, так же потребуется установить и его)
4) Собрать проект с помощью Qt Creator

Системные требования:
1) С++17(STL)
2) Qt6.6.0
3) Qt Creator совместимый с Qt6.6.0
4) MinGw 64 версии 11.2.0 (Qt 6.6.0 MinGw 64 bit)
5) SQLite 3.35.5
6) OpenSSL 3.0.11
7) ВОЗМОЖНО: DB Browser For SQLite 3.12.2
Планы на доработку:
- Перейти с qmake на сmake
- Добавить асинхронное выполнение функций
- Добавить функционал (комбинация "Город + Страна", вывод информации о погодных угрозах и уровне тумана,
  возможность сохранять прогноз для одного города на разное количество дней и сохранять кэш на более продолжительное время
- Улучшить дизайн и интерфейс (Более красивые и динамичные окна)