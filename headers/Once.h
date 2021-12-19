#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <any>

namespace LLDB {
	
	class Session;
	class Row;
	class Results;
	class use_type;
	template <typename T>
	class into_type;
	class into_null_type;

	class Once {

		Session& sess;
		std::ostringstream sql;
		bool executed = false;
		bool set = false;
		int result = -1;
		size_t start = 0;

		void replace(const std::string& k, const std::string& v);
		void replace(const std::string& v);


	public:

		Once(Session& sess);
		
		template<typename T>
		Once& operator<<(T v) {
			sql << v;
			return *this;
		}

		// Replace ${key} or $
		Once& operator,(const use_type& u);

		// Set the result storage & execute
		template <typename T>
		void operator,(into_type<T>& i);
		template <typename T>
		void operator,(into_type<std::vector<T>>& i);
		template <typename T>
		void operator,(into_type<Row>& i);
		template <typename T>
		void operator,(into_type<Results>& i);
		template <typename T>
		void operator,(into_type<bool>& i);
		void operator,(into_null_type);

		std::string getSQL();

	};

	class use_type {

	public:

		bool noKey = false;
		std::string key;
		std::string val;

		use_type(const std::string& k, const std::string& v);
		use_type(const std::string& v);

	};

	template <typename T>
	class into_type {

	public:

		T& val;

		into_type(T& v) : val(v) {}

	};

	class into_null_type {};

	inline std::string to_string(const char* cstr) {
		return std::string(cstr);
	}
	inline std::string to_string(char* cstr) {
		return std::string(cstr);
	}

	template <typename T>
	inline into_type<T> into(T& v) {
		return into_type(v);
	}
	inline into_null_type into() {
		return into_null_type();
	}

	using std::to_string;
	inline use_type use(const std::string& k, const std::string& v) {
		return use_type(k, v);
	}
	template <typename T>
	inline use_type use(const std::string& k, T& v) {
		return use_type(k, to_string(v));
	}
	inline use_type use(const std::string& v) {
		return use_type(v);
	}
	template <typename T>
	inline use_type use(T& v) {
		return use_type(to_string(v));
	}
}