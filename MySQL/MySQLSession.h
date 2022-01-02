#pragma once
#include <Session.h>

struct MYSQL;

namespace LLDB {

    // For Developers: 
    // MySQL Official Documents: 
    // https://dev.mysql.com/doc/c-api/8.0/en/
    class MySQLSession : public Session {

        ::MYSQL* conn = nullptr;

    public:
        
        void open(ConnectionArgs args);
        void close();
        void exec(const std::string& sql);
        void query(const std::string& sql, std::function<bool(Row&, int)> cb);
        void setCurrentDB(const std::string& dbname);
        void changeUser(const std::string& user, const std::string& passwd);

        DatabaseType getType();
        bool isOpen();

    };

}

// Have to extern "C", or GetProcAddress will error
extern "C" {
    LLDBAPI LLDB::Session& newMySQLSession();
}
