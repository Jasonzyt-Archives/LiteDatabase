#pragma once
#include <Session.h>
#include <Utils.h>

struct MYSQL;

namespace LLDB {

	class MySQLSession : public Session {

		MYSQL* db = nullptr;

	public:
		
		// @summary Open a database connection
		// MySQL: 
		//  @params args
		//   - host: The ip/domain of the sql server
		//   - *port: The listening port of the sql server, default 3306
		//   - user: User name
		//   - passwd: Password
		//   - *database: The database which you want to use
		//  @example open({{"host","127.0.0.1"}, {"user","root"}, {"passwd":"114514"}})
		void open(ConnectionArgs args);
		// @summary Close a database connection
		void close();
		// @summary Execute a sql statements without results
		void exec(const std::string& sql);
		// @summary Execute sql statements one by one and fetch rows one by one
		// @note Return false in callback function to stop querying
		// @callback cb Row& The row
		//              int  The number of the statement(0-n)
		void query(const std::string& sql, std::function<bool(Row&, int)> cb) = 0;
		// @summary Execute a sql statement and fetch rows one by one
		// @note Return false in callback function to stop querying
		// @callback cb Row& The row
		void query(const std::string& sql, std::function<bool(Row&)> cb) = 0;
		// @summary Execute a sql statements with results
		Results query(const std::string& sql);

		Once& operator<<(const std::string& sql);

		DatabaseType getType();

	};

}

extern "C" {
	LLDBAPI LLDB::Session& newMySQLSession();
}
