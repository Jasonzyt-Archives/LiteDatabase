#pragma once
#include <unordered_map>
#include <string>
#include <any>

namespace LLDB {
	
	class Row {

		std::unordered_map<std::string, std::any> data;

	public:

		Row();

		// Write
		void emplace(const std::string& header, std::any val);

		// Read
		// Return if there is a header(key) in this row
		bool has(const std::string& header);
		// Get the headers of this row
		std::vector<std::string> headers();
		// Return the value of the header(key). 
		// If doesn't exists, it will return std::any()
		// Tips: 1. Call std::any::type() to get the type of the value
		//       2. Call std::any::_Cast<Type>() to get the value
		std::any get(const std::string& header);
		std::any operator[](const std::string& header);

	};

}