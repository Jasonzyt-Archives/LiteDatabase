#include <Any.h>

namespace LLDB {

	Any::Any() : std::any() {}

	bool Any::isString() {
		if (has_value()) {
			switch (type().hash_code())
			{
			case HASHCODE_CHAR:
			case HASHCODE_CHAR_PTR:
			case HASHCODE_CONST_CHAR_PTR:
			case HASHCODE_STD_STRING:
			case HASHCODE_STD_STRING_PTR:
			case HASHCODE_CONST_STD_STRING_PTR:
				return true;
			}
		}
		return false;
	}
	bool Any::isInteger() {
		if (has_value()) {
			switch (type().hash_code())
			{
			case HASHCODE_CHAR:
			case HASHCODE_UNSIGNED_CHAR:
			case HASHCODE_SHORT:
			case HASHCODE_UNSIGNED_SHORT:
			case HASHCODE_INT:
			case HASHCODE_UNSIGNED_INT:
			case HASHCODE_LONG_LONG:
			case HASHCODE_UNSIGNED_LONG_LONG:
				return true;
			}
		}
		return false;
	}
	bool Any::isFloat() {
		if (has_value()) {
			switch (type().hash_code())
			{
			case HASHCODE_FLOAT:
			case HASHCODE_DOUBLE:
				return true;
			}
		}
		return false;
	}
	bool Any::isNumber() {
		return (isInteger() || isFloat());
	}

	std::string Any::getString() {
		if (has_value()) {
			switch (type().hash_code())
			{
			case HASHCODE_CHAR:
				return std::string() + get<char>();
			case HASHCODE_CHAR_PTR:
				return std::string(get<char*>());
			case HASHCODE_CONST_CHAR_PTR:
				return std::string(get<const char*>());
			case HASHCODE_STD_STRING:
				return get<std::string>();
			case HASHCODE_STD_STRING_PTR:
				return *get<std::string*>();
			case HASHCODE_CONST_STD_STRING_PTR:
				return *get<const std::string*>();
			default:
				return get<std::string>();
			}
		}
		return "";
	}

}