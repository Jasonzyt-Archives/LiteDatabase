#pragma once
#include <Session.h>
#include <Utils.h>

struct sqlite3;

namespace LLDB {

	class SQLiteSession : public Session {

		sqlite3* db = nullptr;

	public:

		// @summary Open a database connection
		// @params 
		//  - target: Target database path or uri(if argument uri == true)
		//  - *create: Create database if the database doesn't exist(default true)
		//  - *readOnly: Enable readOnly mode or not (if argument uri != true, default false)
		//  - *uri: If == true, target will be uri(to get more uri information, plz see https://www.sqlite.org/c3ref/open.html)
		// @example open({{"target","sqlite.db"}, {"create",false}})
		// @note If target == ":memory:", it will create a new database in memory
		//       and it will be released when the connection is closed
		// @throw If fail, throw LLDB::Exception
		void open(ConnectionArgs args);
		// @summary Close a database connection
		void close();
		// @summary Execute a sql statement without results
		void exec(const std::string& sql);
		// @summary Execute sql statements one by one and fetch rows one by one
		// @note Return false in callback function to stop querying
		// @callback cb Row& The row
		//              int  The number of the statement(0-n)
		void query(const std::string& sql, std::function<bool(Row&, int)> cb);
		// @summary Execute a sql statement and fetch rows one by one
		// @note Return false in callback function to stop querying
		// @callback cb Row& The row
		void query(const std::string& sql, std::function<bool(Row&)> cb);
		// @summary Execute a sql statement with results
		Results query(const std::string& sql);

		Once& operator<<(const std::string& sql);

		DatabaseType getType();

	};

}

// Have to extern "C", or GetProcAddress will error
extern "C" {
	LLDBAPI LLDB::Session& newSQLiteSession(); // LLDB::newSession
}
