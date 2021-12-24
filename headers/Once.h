#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <any>
#include <Session.h>
#include <Results.h>

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

		bool heap = false;

		Once(Session& sess);
		
		template<typename T>
		Once& operator<<(T v) {
			sql << v;
			return *this;
		}

		// Replace ${key} or ?
		// char '?' should be "??", or it will error
		Once& operator,(const use_type& u);

		// Set the result storage & execute
		// After using into, the once object will be released!
		template <typename T>
		void operator,(into_type<T>&& i) {
			auto res = sess.query(getSQL());
			if (res.size()) {
				from_row(res[0], i.val);
			}
			release();
		}
		template <typename T>
		void operator,(into_type<std::vector<T>>&& i) {
			auto res = sess.query(getSQL());
			for (auto& row : res) {
				T v;
				from_row(row, v);
				i.val.push_back(v);
			}
			release();
		}
		void operator,(into_type<Row>&& i);
		void operator,(into_type<Results>&& i);
		void operator,(into_type<bool>&& i);
		void operator,(into_null_type);

		std::string getSQL();

		void release();

	};

	void release(Once& once);

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