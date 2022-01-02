#include "SQLiteSession.h"
#include <Results.h>
#include <Once.h>
#include <sqlite3.h>
#include <iostream>

namespace LLDB {

    template <typename ... Args>
    Exception buildException(int code, Args... args) {
        std::ostringstream oss;
        appendStream(oss, args...);
        return Exception(oss.str(), code, DatabaseType::SQLITE);
    }
    Exception buildException(sqlite3* conn) {
        return buildException(sqlite3_errcode(conn), sqlite3_errmsg(conn));
    }

    void SQLiteSession::open(ConnectionArgs args) {
        auto target = args.getString("target"); // Maybe URI
        auto create = args.getBool("create", true, true);
        auto readOnly = args.getBool("readOnly", true, false);
        auto uri = args.getBool("uri", true, false);
        int flags = (readOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE) | 
                    (create ? SQLITE_OPEN_CREATE : 0);
        int res = -1;
        if (target == ":memory:") {
            res = sqlite3_open(":memory:", &conn);
        }
        else if (uri) {
            res = sqlite3_open_v2(target.c_str(), &conn, (create ? SQLITE_OPEN_CREATE : NULL), NULL);
        }
        else {
            res = sqlite3_open_v2(target.c_str(), &conn, flags, NULL);
        }
        if (res != SQLITE_OK || !conn) {
            _isOpen = false;
            throw buildException(conn);
            return;
        }
        _isOpen = true;
    }
    void SQLiteSession::close() {
        if (conn) {
            sqlite3_close(conn);
            conn = 0;
            _isOpen = false;
        }
    }
    void SQLiteSession::exec(const std::string& sql) {
        char* msg;
        auto res = sqlite3_exec(conn, sql.c_str(), NULL, NULL, &msg);
        if (res != SQLITE_OK) {
            throw buildException(res, msg);
        }
    }
    void SQLiteSession::query(const std::string& sql, 
        std::function<bool(Row&, int)> cb) {
        static int stmt_num = 0;
        stmt_num = 0;
        sqlite3_stmt* stmt = nullptr;
        const char* tail = nullptr;
        auto res = sqlite3_prepare_v2(conn, sql.c_str(), -1, &stmt, &tail);
        if (res == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Row row;
                int i = 0;
                char* cColName;
                while (cColName = (char*)sqlite3_column_name(stmt, i)) {
                    std::string colName = cColName;
                    auto tp = sqlite3_column_type(stmt, i);
                    switch (tp)
                    {
                    case SQLITE_BLOB: {
                        row.emplace(colName, (void*)sqlite3_column_blob(stmt, i));
                        break;
                    }
                    case SQLITE_TEXT: {
                        row.emplace(colName,
                            std::string((const char*)sqlite3_column_text(stmt, i)));
                        break;
                    }
                    case SQLITE_INTEGER: {
                        row.emplace(colName, sqlite3_column_int64(stmt, i));
                        break;
                    }
                    // Note: Integer > int64_max will be double
                    case SQLITE_FLOAT: {
                        row.emplace(colName, sqlite3_column_double(stmt, i));
                        break;
                    }
                    case SQLITE_NULL: {
                        row.emplace(colName, Any());
                        break;
                    }
                    }
                    i++;
                }
                if (!cb(row, stmt_num)) {
                    break;
                }
            }
        }
        else {
            sqlite3_finalize(stmt);
            stmt_num = 0;
            throw buildException(conn);
        }
        sqlite3_finalize(stmt);
        if (strlen(tail)) {
            stmt_num++;
            query(tail, cb);
        }
        else {
            stmt_num = 0;
        }
    }

    DatabaseType SQLiteSession::getType() {
        return DatabaseType::SQLITE;
    }
}

LLDB::Session& newSQLiteSession() {
    LLDB::Session* sess = new LLDB::SQLiteSession();
    return *sess;
}