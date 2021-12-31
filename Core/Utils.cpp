#include <Utils.h>
#include <Any.h>

namespace LLDB {
	// Functions
	std::vector<std::string> split(std::string str, char delim) {
		std::vector<std::string> result;
		std::istringstream iss(str);
		std::string line;
		while (std::getline(iss, line, delim)) {
			result.push_back(line);
		}
		return result;
	}

	// class Exception
	Exception::Exception(const char* msg, int errc, DatabaseType dt):
		std::exception(toString(msg, errc, dt).c_str()),
		msg(msg), errcode(errc), dt(dt) {}
	Exception::Exception(const std::string& msg, int errc, DatabaseType dt) :
		std::exception(toString(msg, errc, dt).c_str()),
		msg(msg.c_str()), errcode(errc), dt(dt) {}

	DatabaseType Exception::getDatabaseType() {
		return dt;
	}

	int Exception::code() {
		return errcode;
	}

	const char* Exception::what() {
		return std::exception::what();
	}

	std::string Exception::toString(const std::string& msg, int errcode, 
		DatabaseType dt) {
		std::ostringstream oss;
		if (dt != DatabaseType::UNKNOWN) oss << DatabaseTypeToString(dt);
		else oss << "LLDB";
		if (errcode != -1) oss << '[' << errcode << ']';
		oss << ": " << (msg.empty() ? "Empty exception" : msg);
		return oss.str();
	}

	// class ConnectionArgs
	ConnectionArgs::ConnectionArgs() {}
	ConnectionArgs::ConnectionArgs(std::initializer_list<
		std::pair<const std::string, Any>> il) {
		args.insert(il);
	}

	std::string ConnectionArgs::getString(const std::string& key, bool option) {
		if (!option && !args.count(key)) {
			throw Exception::build("Connection argument not found(string): ", key);
		}
		else if (!args.count(key)) {
			return std::string();
		}
		if (args.at(key).isString()) {
			return args.at(key).getString();
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}

	int ConnectionArgs::getInt(const std::string& key, bool option, int def) {
		if (!option && !args.count(key)) {
			throw Exception::build("Connection argument not found(int): ", key);
		}
		else if (!args.count(key)) {
			return def;
		}
		if (args.at(key).isInteger()) {
			return args.at(key).getInteger<int>();
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}

	bool ConnectionArgs::getBool(const std::string& key, bool option, bool def) {
		if (!option && !args.count(key)) {
			throw Exception("Connection argument not found(bool): " + key);
		}
		else if (!args.count(key)) {
			return def;
		}
		if (args.at(key).is<bool>()) {
			return args.at(key).get<bool>();
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}
}