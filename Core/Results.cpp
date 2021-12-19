#include <Results.h>
#include <Utils.h>
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
		if (index < rows.size()) {
			return rows[index];
		}
		throw Exception::build("Index is too big. length: ", rows.size());
	}
	Row& Results::operator[](size_t index) {
		return get(index);
	}

}