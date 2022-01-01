#pragma once
#include <exception>
#include <Session.h>

namespace LLDB {

    class Any;

    // std::string
    std::vector<std::string> split(std::string str, char delim = ' ');

    class Exception : public std::exception {

        DatabaseType dt = DatabaseType::UNKNOWN;
        int errcode = -1;
        const char* msg;

        static std::string toString(const std::string& msg, int errc, DatabaseType dt);
    
    public:

        Exception(const char* msg, int errc = -1,
            DatabaseType dt = DatabaseType::UNKNOWN);
        Exception(const std::string& msg, int errc = -1,
            DatabaseType dt = DatabaseType::UNKNOWN);
        DatabaseType getDatabaseType();
        int code();
        const char* what();

        template <typename ... Args>
        static Exception build(Args... args);

    };

    class ConnectionArgs {

        std::unordered_map<std::string, Any> args;

    public:

        ConnectionArgs();
        ConnectionArgs(std::initializer_list<std::pair<const std::string, Any>> il);

        std::string getString(const std::string& key, bool option = false, const std::string& def = "");
        int getInt(const std::string& key, bool option = false, int def = -1);
        bool getBool(const std::string& key, bool option = false, bool def = false);

    };

    // Functions
    // Tips: Template functions must delimit in header

    // appandStream
    template <typename Stream, typename T>
    inline void appendStream(Stream& s, T val) {
        s << val;
    }
    template <typename Stream, typename T, typename ... Args>
    inline void appendStream(Stream& s, T val, Args... args) {
        s << val;
        appendStream(s, args...);
    }

    template <typename ... Args>
    static Exception Exception::build(Args... args) {
        std::ostringstream oss;
        appendStream(oss, args...);
        return Exception(oss.str());
    }
}
