#pragma once
#include <ctime>
#include <string>
#include <vector>

struct Date {
    int day;
    int month;
    int year;
    static Date fromString(const std::string& str);
    std::string toString() const;
};

struct Time {
    int hour;
    int minute;
    int second;
    static Time fromString(const std::string& str);
    std::string toString() const;
};

struct DateTime {
    Date date;
    Time time;
    int timeZone = -1; // Pgsql
    static DateTime fromString(const std::string& str);
    std::string toString() const;
};

using TimeStamp = DateTime;

// Type conversion
std::string to_string(const Date& date);
std::string to_string(const Time& time);
std::string to_string(const DateTime& dateTime);

