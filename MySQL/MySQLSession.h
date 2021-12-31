#pragma once
#include <Session.h>
#include <Utils.h>

struct MYSQL;

namespace LLDB {

	// For Developers: 
	// MySQL Official Documents: 
	// https://dev.mysql.com/doc/c-api/8.0/en/
	class MySQLSession : public Session {

		::MYSQL* db = nullptr;

	public:
		
		void open(ConnectionArgs args);
		void close();
		void exec(const std::string& sql);
		void query(const std::string& sql, std::function<bool(Row&, int)> cb);
		void query(const std::string& sql, std::function<bool(Row&)> cb);
		Results query(const std::string& sql);
		void setCurrentDB(const std::string& dbname);
		void changeUser(const std::string& user, const std::string& passwd);

		Once& operator<<(const std::string& sql);
		DatabaseType getType();
		bool isOpen();

	};

}

extern "C" {
	LLDBAPI LLDB::Session& newMySQLSession();
}
