#pragma once
#include <Session.h>

struct sqlite3;

namespace LLDB {

    class SQLiteSession : public Session {

        sqlite3* conn = nullptr;

    public:

        void open(ConnectionArgs args);
        void close();
        void exec(const std::string& sql);
        void query(const std::string& sql, std::function<bool(Row&, int)> cb);
        void query(const std::string& sql, std::function<bool(Row&)> cb);
        Results query(const std::string& sql);

        Once& operator<<(const std::string& sql);

        DatabaseType getType();

    };

}

// Have to extern "C", or GetProcAddress will error
extern "C" {
    LLDBAPI LLDB::Session& newSQLiteSession(); 
}
