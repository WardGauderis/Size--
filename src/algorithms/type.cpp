//
// Created by ward on 11/23/19.
//

#include <algorithm>

#include "type.h"

std::istream& operator>>(std::istream& in, Algorithm& algorithm) {
	std::string token;
	in >> token;
	std::transform(token.begin(), token.end(), token.begin(), ::tolower);
	if (token == "0" || token == "none")
		algorithm = Algorithm::none;
	else if (token == "1" || token == "bisection")
		algorithm = Algorithm::bisection;
	else if (token == "2" || token == "bisection++" || token == "bisectionplusplus")
		algorithm = Algorithm::bisectionPlusPlus;
	else if (token == "3" || token == "lca")
		algorithm = Algorithm::lca;
	else if (token == "4" || token == "olca")
		algorithm = Algorithm::olca;
	else if (token == "5" || token == "repair" || token == "re-pair")
		algorithm = Algorithm::repair;
	else if (token == "6" || token == "sequitur")
		algorithm = Algorithm::sequitur;
	else if (token == "7" || token == "lzw")
		algorithm = Algorithm::lzw;
	else if (token == "8" || token == "sequential")
		algorithm = Algorithm::sequential;
	else
		in.setstate(std::ios_base::failbit);
	return in;
}

std::istream& operator>>(std::istream& in, Mode& mode) {
	std::string token;
	in >> token;
	std::transform(token.begin(), token.end(), token.begin(), ::tolower);
	if (token == "0" || token == "none" || token == "none_specified")
		mode = Mode::none_specified;
	else if (token == "1" || token == "memory" || "memory_efficient")
		mode = Mode::memory_efficient;
	else if (token == "2" || token == "average")
		mode = Mode::average;
	else if (token == "3" || token == "fast")
		mode = Mode::fast;
	else
		in.setstate(std::ios_base::failbit);
	return in;
}

std::ostream& operator<<(std::ostream& os, const Mode& mode) {
	os << static_cast<int>(mode);
	return os;
}