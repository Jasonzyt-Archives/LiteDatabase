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
                return std::string() + _get<char>();
            case HASHCODE_CHAR_PTR:
                return std::string(_get<char*>());
            case HASHCODE_CONST_CHAR_PTR:
                return std::string(_get<const char*>());
            case HASHCODE_STD_STRING:
                return _get<std::string>();
            case HASHCODE_STD_STRING_PTR:
                return *_get<std::string*>();
            case HASHCODE_CONST_STD_STRING_PTR:
                return *_get<const std::string*>();
            default:
                if (is<bool>()) {
                    return (get<bool>() ? "true" : "false");
                }
                if (isInteger()) {
                    if (is<uint64_t>()) {
                        return std::to_string(get<uint64_t>());
                    }
                    return std::to_string(get<int64_t>());
                }
                if (isFloat()) {
                    return std::to_string(get<double>());
                }
                if (type() == typeid(Date)) {
                    return get<Date>().toString();
                }
                if (type() == typeid(Time)) {
                    return get<Time>().toString();
                }
                if (type() == typeid(DateTime)) {
                    return get<DateTime>().toString();
                }
                return _get<std::string>();
            }
        }
        return "";
    }

}