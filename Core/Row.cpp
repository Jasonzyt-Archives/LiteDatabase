#include "Row.h"

namespace LLDB {

	// class Row
	Row::Row() {}

	void Row::emplace(const std::string& header, std::any val) {
		data.emplace(header, val);
	}

	bool Row::has(const std::string& header) {
		return data.count(header);
	}
	std::vector<std::string> Row::headers() {
		std::vector<std::string> result;
		for (auto& [k, _] : data) {
			result.push_back(k);
		}
		return result;
	}
	std::any Row::get(const std::string& header) {
		if (data.count(header)) {
			return data.at(header);
		}
		return std::any();
	}
	std::any Row::operator[](const std::string& header) {
		return get(header);
	}

}