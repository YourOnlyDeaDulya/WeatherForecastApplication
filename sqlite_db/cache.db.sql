BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "CurrentWeather" (
	"city"	TEXT NOT NULL UNIQUE,
	"country"	TEXT NOT NULL,
	"day"	INTEGER NOT NULL,
	"month"	INTEGER NOT NULL,
	"year"	INTEGER NOT NULL,
	"hour"	INTEGER NOT NULL,
	"minute"	INTEGER NOT NULL,
	"weather_condition"	TEXT NOT NULL,
	"celcium_t"	REAL NOT NULL,
	"feels_like"	REAL NOT NULL,
	"is_day"	INTEGER NOT NULL,
	"request_date"	TEXT NOT NULL,
	PRIMARY KEY("city")
);
CREATE TABLE IF NOT EXISTS "ForecastWeather" (
	"city"	TEXT NOT NULL,
	"country"	TEXT NOT NULL,
	"request_date"	TEXT NOT NULL,
	PRIMARY KEY("city")
);
CREATE TABLE IF NOT EXISTS "ForecastDays" (
	"avg_t"	REAL,
	"min_t"	REAL,
	"max_t"	REAL NOT NULL,
	"weather_condition"	TEXT NOT NULL,
	"day"	INTEGER NOT NULL,
	"month"	INTEGER NOT NULL,
	"year"	INTEGER NOT NULL,
	"request_date"	TEXT NOT NULL,
	"city"	TEXT NOT NULL,
	FOREIGN KEY("city") REFERENCES "ForecastWeather"("city") ON DELETE CASCADE
);
INSERT INTO "CurrentWeather" ("city","country","day","month","year","hour","minute","weather_condition","celcium_t","feels_like","is_day","request_date") VALUES ('Москва','Россия',22,11,2023,20,1,'Небольшой снег',-8.0,-14.3,0,'2023-11-22');
INSERT INTO "ForecastWeather" ("city","country","request_date") VALUES ('Москва','Россия','2023-11-22');
INSERT INTO "ForecastDays" ("avg_t","min_t","max_t","weather_condition","day","month","year","request_date","city") VALUES (-7.5,-8.6,-6.5,'Переменная облачность',22,11,2023,'2023-11-22','Москва'),
 (-7.3,-8.6,-6.2,'Умеренный или сильный снег',23,11,2023,'2023-11-22','Москва'),
 (-4.2,-7.1,0.6,'Умеренный или сильный снег',24,11,2023,'2023-11-22','Москва'),
 (-1.9,-4.7,-0.7,'Местами умеренный снег',25,11,2023,'2023-11-22','Москва'),
 (-4.0,-5.3,-2.3,'Дымка',26,11,2023,'2023-11-22','Москва');
COMMIT;
