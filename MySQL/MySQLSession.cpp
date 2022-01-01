#include "MySQLSession.h"
#include <Results.h>
#include <Once.h>
#include <Utils.h>
#include <mysql/mysql.h>

#pragma warning(disable: 26812)

#define OK 0

namespace LLDB {


    template <typename ... Args>
    Exception buildException(int code, Args... args) {
        std::ostringstream oss;
        appendStream(oss, args...);
        return Exception(oss.str(), code, DatabaseType::MYSQL);
    }
    Exception buildException(::MYSQL* conn) {
        return Exception(mysql_error(conn), mysql_errno(conn), DatabaseType::MYSQL);
    }
    
    void MySQLSession::open(ConnectionArgs args) {
        conn = mysql_init(conn);
        auto host = args.getString("host");
        auto port = (uint16_t)args.getInt("port", true, 3306);
        auto db   = args.getString("database", true);
        auto user = args.getString("user");
        auto passwd = args.getString("passwd");
        auto res = mysql_real_connect(conn, host.c_str(), user.c_str(),
            passwd.c_str(), (db.empty() ? NULL : db.c_str()),
            port, NULL, NULL);
        if (res != conn) {
            throw buildException((int)res, "Failed to connect MySQL[",
                host, ':', port, ']');
        }
    }
    void MySQLSession::close() {
        if (conn) {
            mysql_close(conn);
            conn = 0;
        }
    }
    void MySQLSession::exec(const std::string& sql) {
        auto res = mysql_real_query(conn, sql.c_str(), sql.size());
        if (res != OK) {
            throw buildException(conn);
        }
    }
    void MySQLSession::query(const std::string& sql, 
        std::function<bool(Row&, int)> cb) {
        static int stmt_num = 0;
        auto sqls = split(sql, ';');
        for (auto& s : sqls) {
            if (s.empty()) {
                continue;
            }
            auto errc = mysql_real_query(conn, sql.c_str(), sql.size());
            if (errc != OK) {
                throw buildException(conn);
            }
            auto res = mysql_use_result(conn);
            if (res) {
                std::vector<MYSQL_FIELD*> fields;
                MYSQL_FIELD* field = nullptr;
                while ((field = mysql_fetch_field(res))) {
                    fields.push_back(field);
                }
                MYSQL_ROW mrow = nullptr;
                while ((mrow = mysql_fetch_row(res))) {
                    int i = 0;
                    Row row;
                    for (auto& field : fields) {
                        Any val;
                        switch (field->type) {
                        case MYSQL_TYPE_NULL: {
                            break;
                        }
                        // Binary like '01001'
                        case MYSQL_TYPE_BIT: { 
                            uint64_t result = 0;
                            std::string str = mrow[i];
                            for (int i = str.size() - 1; i >= 0; i--) {
                                if (str[i] == '0') {
                                    continue;
                                }
                                else if (str[i] == '1') {
                                    result += pow(2, i);
                                }
                                else {
                                    throw Exception::build("Invalid character '",
                                        str[i], "' in the result");
                                }
                            }
                            val = result;
                            break;
                        }
                        // Date and time
                        case MYSQL_TYPE_DATE: {
                            val = Date::fromString(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_TIME: {
                            val = Time::fromString(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_DATETIME: 
                        case MYSQL_TYPE_TIMESTAMP: {
                            val = DateTime::fromString(mrow[i]);
                            break;
                        }
                        // Decimal
                        case MYSQL_TYPE_DECIMAL: { // May be integer
                            std::string str = mrow[i];
                            if (str.find('.') == std::string::npos) {
                                val = std::stoll(str);
                            }
                            else {
                                val = std::stod(str);
                            }
                            break;
                        }
                        // Float
                        case MYSQL_TYPE_DOUBLE: 
                        case MYSQL_TYPE_FLOAT: {
                            val = std::stod(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_TINY: {
                            val = (char)std::stoi(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_YEAR: {
                            val = std::stoi(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_SHORT: {
                            val = (short)std::stoi(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_INT24: {
                            val = std::stoi(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_LONG: {
                            val = std::stol(mrow[i]);
                            break;
                        }
                        case MYSQL_TYPE_LONGLONG: {
                            val = std::stoll(mrow[i]);
                            break;
                        } 
                        case MYSQL_TYPE_GEOMETRY: 
                        case MYSQL_TYPE_SET: 
                        case MYSQL_TYPE_ENUM: 
                        case MYSQL_TYPE_JSON:
                        case MYSQL_TYPE_BLOB: 
                        case MYSQL_TYPE_TINY_BLOB: 
                        case MYSQL_TYPE_MEDIUM_BLOB:
                        case MYSQL_TYPE_LONG_BLOB: 
                        case MYSQL_TYPE_VARCHAR: 
                        case MYSQL_TYPE_STRING: 
                        case MYSQL_TYPE_VAR_STRING: {
                            val = std::string(mrow[i]);
                            break;
                        }
                        default: {
                            throw Exception::build("Invalid field type!");
                        }
                        }
                        row.emplace(field->name, val);
                        i++;
                    }
                    if (!cb(row, stmt_num)) {
                        break;
                    }
                }
            }
            else {
                throw buildException(conn);
            }
            mysql_free_result(res);
            stmt_num++;
        }
        stmt_num = 0;
    }
    void MySQLSession::query(const std::string& sql, 
        std::function<bool(Row&)> cb) {
        query(sql, [&](Row& row, int) {
            return cb(row);
        });
    }
    Results MySQLSession::query(const std::string& sql) {
        Results result;
        query(sql, [&](Row& row) {
            result.push_back(row);
            return true;
        });
        return result;
    }
    void MySQLSession::setCurrentDB(const std::string& db) {
        if (mysql_select_db(conn, db.c_str()) != OK) {
            throw buildException(conn);
        }
    }
    void MySQLSession::changeUser(const std::string& user, 
        const std::string& passwd) {
        if (mysql_change_user(conn, user.c_str(), passwd.c_str(), NULL) != OK) {
            throw buildException(conn);
        }
    }

    Once& MySQLSession::operator<<(const std::string& sql) {
        Once* once_ptr = new Once(*this);
        Once& once = *once_ptr;
        once.heap = true;
        once << sql;
        return once;
    }

    DatabaseType MySQLSession::getType() {
        return DatabaseType::MYSQL;
    }

    bool MySQLSession::isOpen() {
        if (mysql_ping(conn) == OK) {
            _isOpen = true;
        }
        else {
            _isOpen = false;
        }
        return _isOpen;
    }

}

LLDB::Session& newMySQLSession() {
    LLDB::Session* sess = new LLDB::MySQLSession();
    return *sess;
}