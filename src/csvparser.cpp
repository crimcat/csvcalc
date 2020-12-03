
#include "csvcalc.h"

#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <deque>
#include <stack>
#include <stdexcept>
#include <cstdlib>

namespace {

void
get_line(std::istream *is, std::string *line) {
	if(is && line) {
		std::string s;
		std::getline(*is, s);
		line->clear();
		std::copy_if(s.begin(), s.end(), std::back_inserter(*line), [](char c) { return !std::isspace(c); });
	}
}

void
split_string(const std::string& s, std::vector<std::string> *tokens, char delimiter) {
	size_t lp = 0, rp;
	while(lp < s.length()) {
		for(rp = lp; (rp < s.length()) && (delimiter != s[rp]); ++rp) { }
		tokens->emplace_back(s, lp, rp - lp);
		lp = rp + 1;
	}
}

void
read_csv_header(const std::string& header, std::vector<std::string> *columns, char delimiter) {
	::split_string(header, columns, delimiter);
	if(!(*columns)[0].empty()) {
		throw std::invalid_argument("CSV header must start with void cell");
	}

	columns->erase(columns->begin());
	for(const std::string &cname : *columns) {
		if(0 == cname.length()) {
			throw std::invalid_argument("empty column name");
		}
		for(auto c : cname) {
			if(!std::isalpha(c)) {
				throw std::invalid_argument("bad column name of \'" + cname + "\'");
			}
		}
	}
}

size_t
find_first_punct(const std::string& s, size_t from) {
	for(size_t i = from; i < s.length(); ++i) {
		if(std::ispunct(s[i])) {
			return i;
		}
	}
	return std::string::npos;
}

class expr_solver {
private:
	std::deque<std::string> operands;
	std::deque<char> operations;

	static double checked_stod(const std::string& op) {
		try { return std::stod(op); } catch(std::exception) { throw std::invalid_argument("bad operand " + op); }
	}

	static std::string calc_simple_expr(char op, const std::string& op1, const std::string& op2) {
		double o1 = checked_stod(op1);
		double o2 = checked_stod(op2);
		double res;
		switch(op) {
		case '+': res = o1 + o2; break;
		case '-': res = o1 - o2; break;
		case '*': res = o1 * o2; break;
		case '/':
		if(o2 != 0) {
			res = o1 / o2;
		} else {
			throw std::invalid_argument("division by zero");
		}
		break;
		default:
		throw std::invalid_argument(std::string("bad operation ") + op);
		}
		return std::to_string(res);
	}

public:
	void push_next_operand(const std::string& operand) {
		if(operations.size() == 0) {
			operands.push_back(operand);
		} else {
			char op = operations.back();
			if(('*' == op) || ('/' == op)) {
				operations.pop_back();

				std::string prev_operand = operands.back();
				operands.pop_back();
				operands.push_back(calc_simple_expr(op, prev_operand, operand));
			} else {
				operands.push_back(operand);
			}
		}
	}
	void push_next_operation(char c) {
		operations.push_back(c);
	}

	std::string solve() {
		while(operands.size() > 1) {
			std::string res = calc_simple_expr(operations[0], operands[0], operands[1]);

			operands.pop_front();
			operands.pop_front();
			operations.pop_front();

			operands.push_front(res);
		}
		if((operands.size() == 1) && (operations.size() == 0)) {
			return operands[0];
		}
		throw std::invalid_argument("bad expression");
	}
};

}

namespace csv {

const std::string&
table::evaluate_operand(const std::string& operand) {
	return std::isalpha(operand[0]) ? evaluate_cell_at(operand) : operand;
}

const std::string&
table::evaluate_cell(const std::string& caddr) {
	evaluated_cells_.insert({caddr, ""}); // mark cell under analysis with empty string

	const std::string& cell = cell_at(caddr);
	if('=' != cell[0]) {
		throw std::logic_error("algorithm broken");
	}

	try {
		size_t lp = 1;
		::expr_solver es;
		while(lp < cell.length()) {
			auto rp = ::find_first_punct(cell, lp);
			if(rp == lp) {
				rp = ::find_first_punct(cell, lp + 1);
			}
			if(std::string::npos == rp) {
				es.push_next_operand(evaluate_operand(cell.substr(lp)));
				break;
			}
			es.push_next_operand(evaluate_operand(cell.substr(lp, rp - lp)));
			es.push_next_operation(cell[rp]);
			lp = rp + 1;
		}
		evaluated_cells_.insert_or_assign(caddr, es.solve());
	} catch(const std::invalid_argument& e) {
		evaluated_cells_.erase(caddr);
		throw std::invalid_argument(std::string(e.what()) + " at " + caddr);
	}
	return evaluated_cells_.at(caddr);
}

const std::string&
table::evaluate_cell_at(const std::string& caddr) {
	if(auto it = evaluated_cells_.find(caddr); it != evaluated_cells_.end()) {
		if(it->second.empty()) {
			throw std::invalid_argument("loop at cell: " + caddr);
		}
		return it->second;
	}
	if(auto it = cells_.find(caddr); it != cells_.end()) {
		if('=' != it->second[0]) {
			try {
				std::stod(it->second);
			} catch(std::exception) {
				throw std::invalid_argument("bad const " + it->second); 
			}
			evaluated_cells_.insert({ it->first, it->second });
			return it->second;
		}
		return evaluate_cell(caddr);
	}
	throw std::invalid_argument("unknown cell: " + caddr);
}

table
table::read_csv(std::istream *is, char delimiter) {
	if(!is || !is->good()) {
		throw std::invalid_argument("cannot access input data");
	}

	std::vector<std::string> columns;
	std::vector<std::string> rows;
	std::unordered_map<std::string, std::string> cells;

	std::string line;
	::get_line(is, &line);

	if(is->eof()) {
		throw std::invalid_argument("CSV should contain more than one line");
	}

	::read_csv_header(line, &columns, delimiter);

	while(!is->eof()) {
		::get_line(is, &line);

		if((line.length() == 0)) {
			continue;
		}

		std::vector<std::string> next;
		::split_string(line, &next, delimiter);

		if((next.size() - columns.size()) != 1) {
			throw std::invalid_argument("CSV file has rows of non-equal length");
		}

		for(char c : next[0]) {
			if(!std::isdigit(c)) {
				throw std::invalid_argument("bad row \'" + next[0] + "\' - not a number");
			}
		}

		rows.push_back(next[0]);
		for(size_t i = 0; i < columns.size(); ++i) {
			cells[columns[i] + next[0]] = next[i + 1];
		}
	}

	table t;

	t.columns_ = std::move(columns);
	t.rows_ = std::move(rows);
	t.cells_ = std::move(cells);
	t.evaluated_cells_.clear();

	return t;
}

} // namespace csv
