
#include "csvcalc.h"

#include <iostream>
#include <fstream>
#include <iterator>

void
print_table(csv::table& t) {
	for(const auto& c : t.columns()) {
		std::cout << ',' << c;
	}
	std::cout << '\n';
	for(const auto r : t.rows()) {
		std::cout << r;
		for(const auto c : t.columns()) {
			std::string caddr = c + r;
			try {
				std::cout << ',' << t[caddr];
			} catch(const std::exception& e) {
				std::cout << "[" << e.what() << "]";
			}
		}
		std::cout << '\n';
	}
}

int
main(int argc, char *argv[]) {
	if(argc < 2) {
		std::cout << "Usage:\n\t" << argv[0] << " <csv file name>\n";
	} else {
		std::ifstream in(argv[1]);
		std::optional<csv::table> csv = csv::table::read_csv(&in);
		if(!csv) {
			std::cout << "Error loading table from " << argv[1] << ".\nCheck file presense and its formatting.";
		} else {
			print_table(csv.value());
		}
	}
	
	return 0;
}

