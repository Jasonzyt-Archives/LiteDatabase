#include <SQLiteSession.h>
#include <Results.h>
#include <Once.h>
#include "third-party/sqlite3.h"

namespace LLDB {

	template <typename ... Args>
	Exception buildException(int code, Args... args) {
		std::ostringstream oss;
		appendStream(oss, args...);
		return Exception(oss.str(), code, DatabaseType::SQLITE);
	}
	Exception buildException(sqlite3* db) {
		return buildException(sqlite3_errcode(db), sqlite3_errmsg(db));
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
			res = sqlite3_open(":memory:", &db);
		}
		else if (uri) {
			res = sqlite3_open_v2(target.c_str(), &db, (create ? SQLITE_OPEN_CREATE : NULL), NULL);
		}
		else {
			res = sqlite3_open_v2(target.c_str(), &db, flags, NULL);
		}
		if (res != SQLITE_OK || !db) {
			_isOpen = false;
			throw buildException(db);
			return;
		}
		_isOpen = true;
	}
	void SQLiteSession::close() {
		if (db) {
			sqlite3_close(db);
		}
	}
	void SQLiteSession::exec(const std::string& sql) {
		char* msg;
		auto res = sqlite3_exec(db, sql.c_str(), NULL, NULL, &msg);
		if (res != SQLITE_OK) {
			throw buildException(res, msg);
		}
	}
	Results SQLiteSession::query(const std::string& sql) {
		Results result;
		std::vector<std::string> cols;
		sqlite3_stmt* stmt = NULL;
		const char* tail;
		auto res = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &tail);
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
						auto valStr = (const char*)sqlite3_column_text(stmt, i);
						uint64_t valUll = std::stoull(valStr);
						if (valUll >= INT64_MAX) {
							row.emplace(colName, valUll);
						}
						else {
							int64_t val = sqlite3_column_int64(stmt, i);
							row.emplace(colName, val);
						}
						break;
					}
					case SQLITE_FLOAT: {
						row.emplace(colName, sqlite3_column_double(stmt, i));
						break;
					}
					case SQLITE_NULL: {
						row.emplace(colName, std::any());
						break;
					}
					}
					i++;
				}
				result.push_back(row);
			}
		}
		else {
			sqlite3_finalize(stmt);
			throw buildException(db);
		}
		sqlite3_finalize(stmt);
		return result;
	}
	Results SQLiteSession::selectAll(const std::string& tbl) {
		return query("SELECT * FROM `" + tbl + "`");
	}

	Once& SQLiteSession::operator<<(std::string sql) {
		Once once(*this);
		once << sql;
		return once;
	}

	DatabaseType SQLiteSession::getType() {
		return DatabaseType::SQLITE;
	}
}

LLDB::Session& newSQLiteSession() {
	LLDB::Session* sess = new LLDB::SQLiteSession();
	return *sess;
}