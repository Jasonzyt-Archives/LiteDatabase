#pragma once
#include <unordered_map>
#include <string>
#include <Any.h>

namespace LLDB {
	
	class Row : public std::unordered_map<std::string, Any> {

	public:

		// Read
		// Return if there is a header(key) in this row
		bool has(const std::string& header);
		// Get the headers of this row
		std::vector<std::string> headers();
		// Return the value of the header(key). 
		// If doesn't exists, it will return std::any()
		// Tips: 1. Call std::any::type() to get the type of the value
		//       2. Call std::any::_Cast<Type>() to get the value
		Any get(const std::string& header);
		// = get(header);
		Any operator[](const std::string& header);
		// If the row is empty
		bool empty();
	
	};

}