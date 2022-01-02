#include "Row.h"

namespace LLDB {

    bool Row::has(const std::string& header) {
        return count(header);
    }
    std::vector<std::string> Row::headers() {
        std::vector<std::string> result;
        for (auto& [k, _] : *this) {
            result.push_back(k);
        }
        return result;
    }
    Any Row::get(const std::string& header) {
        if (has(header)) {
            return at(header);
        }
        return Any();
    }
    Any Row::operator[](const std::string& header) {
        return get(header);
    }
    bool Row::empty() {
        return empty();
    }

}