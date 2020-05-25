
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
split_string(const std::string& s, std::vector<std::string> *tokens) {
	size_t lp = 0, rp;
	while(lp < s.length()) {
		for(rp = lp; (rp < s.length()) && (',' != s[rp]); ++rp) { }
		tokens->emplace_back(s.substr(lp, rp - lp));
		lp = rp + 1;
	}
}
bool
read_csv_header(const std::string& header, std::vector<std::string> *columns) {
	::split_string(header, columns);
	if((*columns)[0].empty()) {
		columns->erase(columns->begin());
		return true;
	}
	return false;
}

double
checked_stod(const std::string& op) {
	try { return std::stod(op);	}
	catch(std::exception) { throw std::invalid_argument("Bad operand " + op); }
}

std::string
calc_simple_expr(char op, const std::string& op1, const std::string& op2) {
	// TODO: catch stod exceptions
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
			throw std::invalid_argument("Division by zero");
		}
		break;
	default:
		throw std::invalid_argument("Bad operation " + std::to_string(op));
	}
	return std::to_string(res);
}

void
read_line(std::istream *is, std::string *res) {
	res->clear();
	while(char c = is->get()) {
		if(is->eof()) break;
		if('\n' == c) break;
		if(('\r' != c) && ('\t' != c) && (' ' != c)) {
			res->push_back(c);
		}
	}
}

const char *OPERATIONS = "+-*/!@#$%^&(){}[];:.'\"?<>\\|";

std::string
expr_solver(const std::deque<std::string>& operands, const std::deque<char>& operators) {

	return "0";
}

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
		throw std::logic_error("Algorithm broken");
	}

	// TODO: implement operations priorities
	size_t lp = 1;
	std::deque<std::string> operands;
	std::deque<char> operators;
	while(lp < cell.length()) {
		auto rp = cell.find_first_of(::OPERATIONS, lp);
		if(rp == lp) {
			rp = cell.find_first_of(::OPERATIONS, lp + 1);
		}
		if(std::string::npos == rp) {
			operands.emplace_back(evaluate_operand(cell.substr(lp)));
			break;
		}
		operands.emplace_back(evaluate_operand(cell.substr(lp, rp - lp)));
		operators.emplace_back(cell[rp]);
		lp = rp + 1;
	}

	if(operands.size() - operators.size() != 1) {
		throw std::invalid_argument("Bad expression at " + caddr);
	}

	while(operands.size() > 1) {
		std::string res = ::calc_simple_expr(operators[0], operands[0], operands[1]);

		operands.pop_front();
		operands.pop_front();
		operators.pop_front();

		operands.push_front(res);
	}
	if((operands.size() != 1) && (operators.size() != 0)) {
		throw std::invalid_argument("Bad expression at " + caddr);
	}

	evaluated_cells_.insert_or_assign(caddr, operands.front());
	return evaluated_cells_.at(caddr);
}

const std::string&
table::evaluate_cell_at(const std::string& caddr) {
	if(auto it = evaluated_cells_.find(caddr); it != evaluated_cells_.end()) {
		if(it->second.empty()) {
			throw std::invalid_argument("Loop at cell address: " + caddr);
		}
		return it->second;
	}
	if(auto it = cells_.find(caddr); it != cells_.end()) {
		if('=' != it->second[0]) {
			evaluated_cells_.insert({ it->first, it->second });
			return it->second;
		}
		return evaluate_cell(caddr);
	}
	throw std::invalid_argument("Unknown cell address: " + caddr);
}

bool
table::load(std::istream *is) {
	if(is && is->good()) {
		std::vector<std::string> columns;
		std::vector<std::string> rows;
		std::unordered_map<std::string, std::string> cells;

		std::string line;

		::read_line(is, &line);
		if(!is->eof() && ::read_csv_header(line, &columns)) {
			while(!is->eof()) {
				::read_line(is, &line);
				
				if((line.length() == 0)) {
					continue;
				}

				std::vector<std::string> next;
				::split_string(line, &next);

				if((next.size() - columns.size()) != 1) {
					return false;
				}

				rows.push_back(next[0]);
				for(size_t i = 0; i < columns.size(); ++i) {
					cells[columns[i] + next[0]] = next[i + 1];
				}
			}

			columns_ = std::move(columns);
			rows_ = std::move(rows);
			cells_ = std::move(cells);
			evaluated_cells_.clear();

			return true;
		}
	}
	return false;
}

} // namespace csv
