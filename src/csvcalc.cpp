
#include "csvcalc.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>
#include <functional>
#include <cstring>

void
print_table(csv::table& t) {
	for(const auto& c : t.columns()) {
		std::cout << ',' << c;
	}
	std::cout << '\n';
	for(const auto& r : t.rows()) {
		std::cout << r;
		for(const auto& c : t.columns()) {
			try {
				std::cout << ',' << t[c + r];
			} catch(const std::exception& e) {
				std::cout << "[" << e.what() << "]";
			}
		}
		std::cout << '\n';
	}
}

int
main(int argc, char *argv[]) {
	if(argc != 2) {
		std::cout << "Usage:\n\t" << argv[0] << " <csv file name>\n";
	} else {
		const bool is_stdin = (0 == std::strcmp("-", argv[1]));
		std::istream *csv_is = is_stdin
			? &std::cin
			: new std::ifstream(argv[1]);

		try {
			csv::table csv = csv::table::read_csv(csv_is);
			print_table(csv);
		} catch(const std::exception &e) {
			std::cout << "Error loading table: " << e.what() << '\n';
		}

		if(!is_stdin) {
			delete csv_is;
		}
	}
	return 0;
}

