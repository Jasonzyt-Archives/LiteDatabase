#include <Utils.h>

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

	std::string getString(std::any val) {
		if (val.has_value()) {
			switch (val.type().hash_code())
			{
			case HASHCODE_CHAR:
				return std::string() + get<char>(val);
			case HASHCODE_CHAR_PTR:
				return std::string(get<char*>(val));
			case HASHCODE_CONST_CHAR_PTR:
				return std::string(get<const char*>(val));
			case HASHCODE_STD_STRING:
				return get<std::string>(val);
			case HASHCODE_STD_STRING_PTR:
				return *get<std::string*>(val);
			case HASHCODE_CONST_STD_STRING_PTR:
				return *get<const std::string*>(val);
			default:
				return get<std::string>(val);
			}
		}
		return "";
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
		std::pair<const std::string, std::any>> il) {
		args.insert(il);
	}

	std::string ConnectionArgs::getString(const std::string& key, bool option) {
		if (!option && !args.count(key))
			throw Exception::build("Connection argument not found(string): ", key);
		if (is<std::string>(args.at(key))) {
			return get<std::string>(args.at(key));
		}
		else if (is<const char*>(args.at(key))) {
			return get<const char*>(args.at(key));
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}

	int ConnectionArgs::getInt(const std::string& key, bool option, int def) {
		if (!option && !args.count(key))
			throw Exception::build("Connection argument not found(int): ", key);
		else if (!args.count(key)) {
			return def;
		}
		if (is<int>(args.at(key))) {
			return get<int>(args.at(key));
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}

	bool ConnectionArgs::getBool(const std::string& key, bool option, bool def) {
		if (!option && !args.count(key))
			throw Exception("Connection argument not found(bool): " + key);
		else if (!args.count(key)) {
			return def;
		}
		if (is<bool>(args.at(key))) {
			return get<bool>(args.at(key));
		}
		else {
			throw Exception::build("The type of connection argument '", key, "' is wrong!");
		}
	}
}