#pragma once
#include <any>
#include <string>
#include <TypeIDs.h>
#include <Utils.h>
#include <Types.h>

namespace LLDB {

	class Any : public std::any {

		template <typename T>
		inline T _get() {
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

	public:
		
		Any();
		template <typename T>
		Any(T val) : std::any(val) {}

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
					return (T)_get<char>();
				case HASHCODE_UNSIGNED_CHAR:
					return (T)_get<unsigned char>();
				case HASHCODE_SHORT:
					return (T)_get<short>();
				case HASHCODE_UNSIGNED_SHORT:
					return (T)_get<unsigned short>();
				case HASHCODE_INT:
					return (T)_get<int>();
				case HASHCODE_UNSIGNED_INT:
					return (T)_get<unsigned int>();
				case HASHCODE_LONG_LONG:
					return (T)_get<long long>();
				case HASHCODE_UNSIGNED_LONG_LONG:
					return (T)_get<unsigned long long>();
				case HASHCODE_FLOAT:
					return (T)_get<float>();
				case HASHCODE_DOUBLE:
					return (T)_get<double>();
				case HASHCODE_BOOL:
					return (T)_get<bool>();
				case HASHCODE_LONG:
					return (T)_get<long>();
				case HASHCODE_UNSIGNED_LONG:
					return (T)_get<long>();
				default:
					return _get<T>();
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
					return (T)_get<float>();
				case HASHCODE_DOUBLE:
					return (T)_get<double>();
				default:
					return _get<T>();
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

		template <typename T>
		inline T get() {
			return _get<T>();
		}
		// Special
		template <>
		inline std::string get<std::string>() {
			return getString();
		}
		template <>
		inline const char* get<const char*>() {
			auto&& str = getString();
			return str.c_str();
		}
		template <>
		inline char* get<char*>() {
			return const_cast<char*>(get<const char*>());
		}
		template <>
		inline char get<char>() {
			return getNumber<char>();
		}
		template <>
		inline short get<short>() {
			return getNumber<short>();
		}
		template <>
		inline int get<int>() {
			return getNumber<int>();
		}
		template <>
		inline long long get<long long>() {
			return getNumber<long long>();
		}
		template<>
		inline unsigned char get<unsigned char>() {
			return getNumber<unsigned char>();
		}
		template <>
		inline unsigned short get<unsigned short>() {
			return getNumber<unsigned short>();
		}
		template <>
		inline unsigned int get<unsigned int>() {
			return getNumber<unsigned int>();
		}
		template <>
		inline unsigned long long get<unsigned long long>() {
			return getNumber<unsigned long long>();
		}
		template <>
		inline float get<float>() {
			return getNumber<float>();
		}
		template <>
		inline double get<double>() {
			return getNumber<double>();
		}

	};

}