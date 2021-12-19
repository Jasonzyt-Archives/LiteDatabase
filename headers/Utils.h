#pragma once
#include <exception>
#include <any>
#include <Session.h>
#include <TypeIDs.h>

namespace LLDB {

	// std::string
	std::vector<std::string> split(std::string str, char delim = ' ');

	class Exception : public std::exception {

		DatabaseType dt = DatabaseType::UNKNOWN;
		int errcode = -1;
		const char* msg;

		static std::string toString(const std::string& msg, int errc, DatabaseType dt);
	
	public:

		Exception(const char* msg, int errc = -1,
			DatabaseType dt = DatabaseType::UNKNOWN);
		Exception(const std::string& msg, int errc = -1,
			DatabaseType dt = DatabaseType::UNKNOWN);
		DatabaseType getDatabaseType();
		int code();
		const char* what();

		template <typename ... Args>
		static Exception build(Args... args);

	};

	class ConnectionArgs {

		std::unordered_map<std::string, std::any> args;

	public:

		ConnectionArgs();
		ConnectionArgs(std::initializer_list<std::pair<const std::string, std::any>> il);

		std::string getString(const std::string& key, bool option = false);
		int getInt(const std::string& key, bool option = false, int def = -1);
		bool getBool(const std::string& key, bool option = false, bool def = false);

	};

	// Functions
	// Tips: Template functions must delimit in header

	// appandStream
	template <typename Stream, typename T>
	inline void appendStream(Stream& s, T val) {
		s << val;
	}
	template <typename Stream, typename T, typename ... Args>
	inline void appendStream(Stream& s, T val, Args... args) {
		s << val;
		appendStream(s, args...);
	}

	// std::any
	template <typename T>
	inline bool is(std::any val) {
		return (val.type() == typeid(T));
	}
	template <typename T>
	inline T get(std::any val) {
		if (val.has_value()) {
			//return std::any_cast<T>(val);
			auto ptr = val._Cast<T>();
			if (ptr) {
				return *ptr;
			}
			throw Exception::build("LLDB::get: Failed to get, source type(", 
				val.type().name(), "), target type(", typeid(T).name(), ')');
		}
		throw Exception::build("LLDB::get: val is empty!!!");
	}
	// Only for string
	std::string getString(std::any val);
	// Only for integer like int,short
	template <typename T>
	inline T getInteger(std::any val) {
		if (val.has_value()) {
			switch (val.type().hash_code())
			{
			case HASHCODE_CHAR:
				return (T)get<char>(val);
			case HASHCODE_UNSIGNED_CHAR:
				return (T)get<unsigned char>(val);
			case HASHCODE_SHORT:
				return (T)get<short>(val);
			case HASHCODE_UNSIGNED_SHORT:
				return (T)get<unsigned short>(val);
			case HASHCODE_INT:
				return (T)get<int>(val);
			case HASHCODE_UNSIGNED_INT:
				return (T)get<unsigned int>(val);
			case HASHCODE_LONG_LONG:
				return (T)get<long long>(val);
			case HASHCODE_UNSIGNED_LONG_LONG:
				return (T)get<unsigned long long>(val);
			case HASHCODE_FLOAT:
				return (T)get<float>(val);
			case HASHCODE_DOUBLE:
				return (T)get<double>(val);
			default:
				return get<T>(val);
			}
		}
		return 0;
	}

	template <typename ... Args>
	static Exception Exception::build(Args... args) {
		std::ostringstream oss;
		appendStream(oss, args...);
		return Exception(oss.str());
	}
}