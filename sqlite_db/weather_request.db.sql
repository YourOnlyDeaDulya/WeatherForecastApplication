BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "CurrentWeather" (
	"city"	TEXT,
	"country"	TEXT,
	"day"	INTEGER,
	"month"	INTEGER,
	"year"	INTEGER,
	"hour"	INTEGER,
	"minute"	INTEGER,
	"weather_condition"	TEXT,
	"celcium_t"	REAL,
	"feels_like"	REAL,
	"is_day"	INTEGER
);
CREATE TABLE IF NOT EXISTS "ForecastWeather" (
	"city"	TEXT,
	"country"	TEXT
);
CREATE TABLE IF NOT EXISTS "ForecastDays" (
	"avg_t"	REAL,
	"min_t"	REAL,
	"max_t"	REAL,
	"weather_condition"	TEXT,
	"day"	INTEGER,
	"month"	INTEGER,
	"year"	INTEGER
);
INSERT INTO "CurrentWeather" ("city","country","day","month","year","hour","minute","weather_condition","celcium_t","feels_like","is_day") VALUES ('Москва','Россия',16,11,2023,20,51,'Пасмурно',0.0,-3.7,0);
INSERT INTO "ForecastWeather" ("city","country") VALUES ('Вашингтон','Соединенные Штаты Америки');
INSERT INTO "ForecastDays" ("avg_t","min_t","max_t","weather_condition","day","month","year") VALUES (12.7,4.9,21.0,'Солнечно',16,11,2023),
 (13.9,10.3,19.7,'Переменная облачность',17,11,2023),
 (11.8,3.9,13.6,'Местами дождь',18,11,2023),
 (8.2,0.6,14.7,'Солнечно',19,11,2023),
 (7.7,5.2,10.8,'Облачно',20,11,2023),
 (7.2,5.6,7.3,'Умеренный дождь',21,11,2023),
 (12.3,7.6,15.0,'Умеренный дождь',22,11,2023),
 (4.0,-1.0,6.3,'Солнечно',23,11,2023),
 (-0.6,-3.3,2.5,'Солнечно',24,11,2023),
 (1.9,-0.5,3.9,'Местами дождь',25,11,2023);
COMMIT;
