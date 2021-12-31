#include <Session.h>
#include <Utils.h>
#include <Config.h>
#include <Windows.h>
#include <functional>
#include <filesystem>

// getenv
#pragma warning(disable:4996)

namespace fs = std::filesystem;

namespace LLDB {

	Exception NotSupportedException() {
		return Exception::build
			("This kind of database doesn't support this operation");
	}

	std::string DatabaseTypeToString(DatabaseType type) {
		switch (type) {
		case DatabaseType::SQLITE:
			return DT_SQLITE;
		case DatabaseType::MYSQL:
			return DT_MYSQL;
		case DatabaseType::POSTGRESQL:
			return DT_PGSQL;
		case DatabaseType::ORACLE:
			return DT_ORACLE;
		case DatabaseType::ODBC:
			return DT_ODBC;
		default:
			return DT_UNKNOWN;
		}
	}

	void Session::setCurrentDB(const std::string& dbname) {
		throw NotSupportedException();
	}

	bool Session::isOpen() {
		return _isOpen;
	}

	void Session::release() {
		LLDB::release(*this);
	}

	std::string searchEnvironment(const std::string& lib) {
		auto envStr = getenv("Path");
		auto env = split(envStr, ';');
		for (auto& pa : env) {
			std::string path = pa;
			if (path.back() != '\\' && path.back() != '/') {
				path += '\\';
			}
			path += lib;
			if (fs::exists(path) && !fs::is_directory(path)) {
				return path;
			}
		}
		return std::string();
	}
	Session& newSession(DatabaseType dt) {
		auto type = DatabaseTypeToString(dt);
		auto dllName = DLL_PREFIX + type + DLL_SUFFIX;
		if (!CREATION_FUNCTIONS.count(type) || (int)dt <= 0 || (int)dt > 5) {
			throw Exception::build("Unknown DatabaseType: ", (int)dt);
		}
		auto funcName = CREATION_FUNCTIONS.at(type);
		auto path = searchEnvironment(dllName);
		if (path.empty()) {
			if (fs::exists(dllName)) {
				path = ".\\" + dllName;
			}
			else {
				throw Exception::build("Dynamic link library(", dllName, 
					") not found!");
			}
		}
		HMODULE handler = LoadLibraryA(path.c_str());
		if (!handler) {
			throw Exception::build("Error when loading dynamic link library: ",
				dllName, " ErrCode: ", GetLastError());
		}
		auto func = GetProcAddress(handler, funcName.c_str());
		if (!func) {
			throw Exception::build("Error when getting proc address: ", funcName);
		}
		return ((Session & (*)())func)();
	}

	void release(Session& sess) {
		sess.close();
		Session* ptr = &sess;
		delete ptr;
		ptr = 0;
	}

}