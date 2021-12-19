#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <Once.h>

namespace LLDB {

	class Results;
	class ConnectionArgs;

	enum class DatabaseType {
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

		bool _isOpen;

	public:

		virtual void open(ConnectionArgs args) = 0;
		virtual void close() = 0;
		virtual void exec(const std::string& sql) = 0;
		virtual Results query(const std::string& sql) = 0;
		virtual Results selectAll(const std::string& tbl) = 0;

		virtual Once& operator<<(std::string sql) = 0;

		virtual DatabaseType getType() = 0;
		bool isOpen();

	};

	Session& newSession(DatabaseType dt);

}