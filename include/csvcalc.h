#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

namespace csv {

class table {
private:
	std::vector<std::string> rows_;
	std::vector<std::string> columns_;
	std::unordered_map<std::string, std::string> cells_;
	std::unordered_map<std::string, std::string> evaluated_cells_;

	const std::string& evaluate_operand(const std::string& operand);
	const std::string& evaluate_cell(const std::string& caddr);

	table() { }
public:
	table(const table&) = default;
	table(table&&) = default;
	virtual ~table() = default;
	
	table& operator=(const table&) = default;

	static table read_csv(std::istream *is, char delimiter = ',');
	
	const std::vector<std::string>& rows() const { return rows_; }
	const std::vector<std::string>& columns() const { return columns_; }

	const std::string& cell_at(const std::string& caddr) const {
		return cells_.at(caddr);
	}

	const std::string& evaluate_cell_at(const std::string& caddr);
	const std::string& operator[](const std::string& caddr) {
		return evaluate_cell_at(caddr);
	}
};

} // namespace csv