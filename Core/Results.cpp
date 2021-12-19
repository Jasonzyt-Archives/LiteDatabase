#include "Results.h"

namespace LLDB {

	// class Results
	void Results::push_back(Row row) {
		rows.push_back(row);
	}

	size_t Results::size() {
		return rows.size();
	}
	std::vector<Row>& Results::getAll() {
		return rows;
	}
	Row& Results::get(size_t index) {
		return rows[index];
	}
	Row& Results::operator[](size_t index) {
		return get(index);
	}

}