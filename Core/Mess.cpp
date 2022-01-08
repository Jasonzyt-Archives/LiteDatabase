#include <sstream>
#include <algorithm>
#include <Types.h>
#include <Utils.h>

// Type conversion
Date Date::fromString(const std::string& str) {
    Date date;
    std::stringstream ss(str);
    ss >> date.year;
    ss.ignore();
    ss >> date.month;
    ss.ignore();
    ss >> date.day;
    return date;
}
std::string Date::toString() const {
    return to_string(*this);
}

Time Time::fromString(const std::string& str) {
    Time time;
    std::stringstream ss(str);
    ss >> time.hour;
    ss.ignore();
    ss >> time.minute;
    ss.ignore();
    ss >> time.second;
    return time;
}
std::string Time::toString() const {
    return to_string(*this);
}

DateTime DateTime::fromString(const std::string& str) {
    DateTime dateTime;
    auto s = LLDB::split(str, ' ');
    dateTime.date = Date::fromString(s[0]);
    if (s[1].find_last_of('-') != std::string::npos) {
        auto t = LLDB::split(s[1], '-');
        dateTime.time = Time::fromString(t[0]);
        dateTime.timeZone = std::stoi(t[1]);
    } else {
    dateTime.time = Time::fromString(s[1]);
    }
    return dateTime;
}
std::string DateTime::toString() const {
    return to_string(*this);
}

std::string to_string(const Date& date) {
    std::stringstream ss;
    ss << date.year << "-" << date.month << "-" << date.day;
    return ss.str();
}

std::string to_string(const Time& time) {
    std::stringstream ss;
    ss << time.hour << ":" << time.minute << ":" << time.second;
    return ss.str();
}

std::string to_string(const DateTime& dateTime) {
    std::stringstream ss;
    ss << to_string(dateTime.date) << " " << to_string(dateTime.time);
    if (dateTime.timeZone != -1) {
        ss << "-" << dateTime.timeZone;
    }
    return ss.str();
}
