#pragma once
#include <any>
#include <string>
#include <Utils.h>
#include <TypeIDs.h>

namespace LLDB {
	
	class Any : public std::any {

	public:
		
		Any();
		template <typename T>
		Any(T val) : std::any(val) {}

		template <typename T>
		inline T& get() {
			if (has_value()) {
				//return std::any_cast<T>(val);
				auto ptr = _Cast<T>();
				if (ptr) {
					return *ptr;
				}
				throw Exception::build("LLDB::get: Failed to get, source type(",
					type().name(), "), target type(", typeid(T).name(), ')');
			}
			throw Exception::build("LLDB::get: val is empty!!!");
		}

		template <typename T>
		inline bool is() {
			return (type() == typeid(T));
		}

		bool isString();
		bool isInteger();
		bool isFloat();
		bool isNumber();

		std::string getString();
		template <typename T = int>
		inline T getInteger() {
			if (has_value()) {
				switch (type().hash_code())
				{
				case HASHCODE_CHAR:
					return (T)get<char>();
				case HASHCODE_UNSIGNED_CHAR:
					return (T)get<unsigned char>();
				case HASHCODE_SHORT:
					return (T)get<short>();
				case HASHCODE_UNSIGNED_SHORT:
					return (T)get<unsigned short>();
				case HASHCODE_INT:
					return (T)get<int>();
				case HASHCODE_UNSIGNED_INT:
					return (T)get<unsigned int>();
				case HASHCODE_LONG_LONG:
					return (T)get<long long>();
				case HASHCODE_UNSIGNED_LONG_LONG:
					return (T)get<unsigned long long>();
				default:
					return get<T>();
				}
			}
			return 0;
		}
		template <typename T = double>
		inline T getFloat() {
			if (has_value()) {
				switch (type().hash_code())
				{
				case HASHCODE_FLOAT:
					return (T)get<float>();
				case HASHCODE_DOUBLE:
					return (T)get<double>();
				default:
					return get<T>();
				}
			}
			return 0;
		}
		template <typename T = int>
		inline T getNumber() {
			if (isFloat()) {
				return getFloat<T>();
			}
			return getInteger<T>();
		}

	};

}