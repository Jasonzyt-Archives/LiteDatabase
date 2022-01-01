#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <functional>

#define LLDBAPI __declspec(dllexport)
#pragma warning(disable: 26812)

namespace LLDB {

    class Once;
    class Results;
    class Row;
    class ConnectionArgs;

    enum DatabaseType : char {
        UNKNOWN,
        SQLITE,
        MYSQL,
        POSTGRESQL,
        ORACLE,
        ODBC
    };
    std::string DatabaseTypeToString(DatabaseType type);

    class Session {
        
    protected:

        bool _isOpen = false;

    public:

        // @summary Open a database connection
        // SQLite: 
        //  @params args
        //   - target: Target database path or uri(if argument uri == true)
        //   - *create: Create database if the database doesn't exist(default true)
        //   - *readOnly: Enable readOnly mode or not (if argument uri != true, default false)
        //   - *uri: If == true, target will be uri(to get more uri information, plz see https://www.sqlite.org/c3ref/open.html)
        //  @example open({{"target","sqlite.db"}, {"create",false}})
        //  @note If target == ":memory:", it will create a new database in memory
        //        and it will be released when the connection is closed
        // 
        // MySQL: 
        //  @params args
        //   - host: The ip/domain of the sql server
        //   - *port: The listening port of the sql server, default 3306
        //   - user: User name
        //   - passwd: Password
        //   - *database: The database which you want to use
        //  @example open({{"host","127.0.0.1"}, {"user","root"}, {"passwd":"114514"}})
        virtual void open(ConnectionArgs args) = 0;
        // @summary Close a database connection
        virtual void close() = 0;
        // @summary Execute a sql statement without results
        virtual void exec(const std::string& sql) = 0;
        // @summary Execute sql statements one by one and fetch rows one by one
        // @note Return false in callback function to stop querying
        // @callback cb Row& The row
        //              int  The number of the statement(0-n)
        virtual void query(const std::string& sql, std::function<bool(Row&, int)> cb) = 0;
        // @summary Execute a sql statement and fetch rows one by one
        // @note Return false in callback function to stop querying
        // @callback cb Row& The row
        virtual void query(const std::string& sql, std::function<bool(Row&)> cb) = 0;
        // @summary Execute a sql statement with results
        virtual Results query(const std::string& sql) = 0;
        // @summary Change the current Database
        virtual void setCurrentDB(const std::string& dbname);
        // @summary Change the user
        virtual void changeUser(const std::string& user, const std::string& passwd);

        // @summary Stream API to execute sql statements
        // @example sess << "SQL" << "A$ $$ ${key}", use("B"), use("key", "val"), into();
        //          This is equivalent to exec("SQLAB $ val")
        virtual Once& operator<<(const std::string& sql) = 0;
        
        // @summary Get the database type
        virtual DatabaseType getType() = 0;
        // @summary Return if the connection is opening
        bool isOpen();
        // @summary Close the connection and release the memory
        // @warning Don't use the members afther releasing
        void release();

    };

    // @summary Create a new Session object
    Session& newSession(DatabaseType dt);

    void release(Session& sess);

}