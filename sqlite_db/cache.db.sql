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

COMMIT;
