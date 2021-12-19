#pragma once
#include <vector>
#include "Row.h"

namespace LLDB {

	class Results {
		
		std::vector<Row> rows;

	public:

		// Write
		void push_back(Row row);

		// Read
		size_t size();
		std::vector<Row>& getAll();
		Row& get(size_t index);
		Row& operator[](size_t index);

	};

}