#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <charconv>
#include <sstream>
#include <cstdlib>

namespace csv {

class table {
private:
	std::vector<std::string> rows_;
	std::vector<std::string> columns_;
	std::unordered_map<std::string, std::string> cells_;
	std::unordered_map<std::string, std::string> evaluated_cells_;

	table(const table&) = delete;
	const table& operator=(const table&) = delete;

	const std::string& evaluate_operand(const std::string& operand);
	const std::string& evaluate_cell(const std::string& caddr);

public:
	table() { }
	
	bool load(std::istream *is);

	const std::vector<std::string>& rows() const { return rows_; }
	const std::vector<std::string>& columns() const { return columns_; }

	const std::string& cell_at(const std::string& caddr) const {
		return cells_.at(caddr);
	}

	const std::string& evaluate_cell_at(const std::string& caddr);
};

} // namespace csv