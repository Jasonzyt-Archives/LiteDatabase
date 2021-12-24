#include "MySQLSession.h"
#include <Results.h>
#include <mysql/mysql.h>

namespace LLDB {


	template <typename ... Args>
	Exception buildException(int code, Args... args) {
		std::ostringstream oss;
		appendStream(oss, args...);
		return Exception(oss.str(), code, DatabaseType::MYSQL);
	}
	Exception buildException(MYSQL* db) {
		return Exception(mysql_error(db), mysql_errno(db), DatabaseType::MYSQL);
	}
	
	void MySQLSession::open(ConnectionArgs args) {
		mysql_init(db);
		auto host = args.getString("host");
		auto port = (uint16_t)args.getInt("port", true, 3306);
		auto dbname = args.getString("database", true);
		auto user = args.getString("user");
		auto passwd = args.getString("passwd");
		auto res = mysql_real_connect(db, host.c_str(), user.c_str(),
			passwd.c_str(), (dbname.empty() ? NULL : dbname.c_str()),
			port, NULL, NULL);
		if (res != db) {
			throw buildException((int)res, "Failed to connect MySQL[",
				host, ':', port, ']');
		}
	}
	void MySQLSession::close() {
		if (db) {
			mysql_close(db);
			db = 0;
		}
	}
	void MySQLSession::exec(const std::string& sql) {
		auto res = mysql_real_query(db, sql.c_str(), sql.size());
		if (res != CR_OK) {
			throw buildException(db);
		}
		// Get result: mysql_store_result(db);
		// https://dev.mysql.com/doc/c-api/8.0/en/mysql-fetch-row.html
	}
	void MySQLSession::query(const std::string& sql, std::function<bool(Row&, int)> cb) {
		static int stat_num = 0;
		auto sqls = split(sql, ';');
		for (auto& s : sqls) {
			if (s.empty()) {
				continue;
			}
			auto stmt = mysql_stmt_init(db);
			if (mysql_stmt_prepare(stmt, sql.c_str(), sql.size()) == CR_OK) {
				if (mysql_stmt_execute(stmt) == CR_OK) {
					// auto rows = mysql_stmt_affected_rows(stmt);
					while (mysql_stmt_next_result(stmt) == 0) {
					}
				}
			}
			stat_num++;
		}
		stat_num = 0;
	}
	Results MySQLSession::query(const std::string& sql) {
		if (mysql_real_query(db, sql.c_str(), sql.size()) == CR_OK) {
			auto res = mysql_store_result(db);
			auto rows = mysql_num_rows(res);
		}
	}

	DatabaseType MySQLSession::getType() {
		return DatabaseType::MYSQL;
	}

}

LLDB::Session& newMySQLSession() {
	LLDB::Session* sess = new LLDB::MySQLSession();
	return *sess;
}