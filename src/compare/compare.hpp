//============================================================================
// @name        : palDecoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     :
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description :
//============================================================================

#include <fstream>
#include <chrono>
#include "../pal.h"

namespace compare {
	std::string getAlgoName(int i) {
		switch (i) {
			case 0:
				return "None";
			case 1:
				return "Bisection";
			case 2:
				return "Bisection++";
			case 3:
				return "LCA";
			case 4:
				return "OLCA";
			case 5:
				return "Repair";
			case 6:
				return "Sequitur";
			case 7:
				return "LZW";
			default:
				return "something went wrong";
		}
	}

	Algorithm getAlgoType(int i) {
		switch (i) {
			case 0:
				return Algorithm::none;
			case 1:
				return Algorithm::bisection;
			case 2:
				return Algorithm::bisectionPlusPlus;
			case 3:
				return Algorithm::lca;
			case 4:
				return Algorithm::olca;
			case 5:
				return Algorithm::repair;
			case 6:
				return Algorithm::sequitur;
			case 7:
				return Algorithm::lzw;
		}
		return Algorithm::none;
	}

	void compareAlgorithms(std::string filename, std::string output) {
		int final{};
		for (int i = filename.size(); i >= 0; i--) {
			if (filename[i] == '/') break;
			final = i;
		}

		uint32_t minsize = std::numeric_limits<uint32_t>::max();
		uint32_t mintime = std::numeric_limits<uint32_t>::max();
		uint32_t mintimealgo{};
		uint32_t minsizealgo{};

		std::string name = filename.substr(final, filename.size() - final);

		// create table
		std::fstream file;
		file.open("./comparison_" + output + ".html", std::fstream::out);

		file << "<style>\n"
		        "table {\n"
		        "   border-collapse: collapse;\n"
		        "}\n"
		        "td, th {\n"
		        "   border: 1px solid orange;\n"
		        "   padding: 15px;\n"
		        "}\n"
		        "</style>\n";

		file << "Comparison for file: " << output;
		file << "<br>Original size: " << std::filesystem::file_size(filename) << "B";
		file << "<table>";
		for (int i = -1; i <= 7; i++) {
			file << "<tr>";

			std::chrono::time_point start = std::chrono::high_resolution_clock::now();
			if (i != -1) {
				std::cout << "Running: " << getAlgoName(i) << std::endl;
				pal::encode(filename, output, getAlgoType(i), Mode::none_specified, false, false, false);
			}
			std::chrono::time_point end = std::chrono::high_resolution_clock::now();
			uint32_t dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


			if (dur < mintime and i != 0 and i != -1) {
				mintimealgo = i;
				mintime = dur;
			}
			uint32_t size = std::numeric_limits<uint32_t>::max();
			if (i != -1) size = std::filesystem::file_size(output);

			if (size < minsize and i != -1) {
				minsizealgo = i;
				minsize = size;
			}

			for (int j = 0; j <= 3; j++) {
				file << "<th>";
				if (i == -1) {
					switch (j) {
						case 0:
							file << "algorithm";
							break;
						case 1:
							file << "size";
							break;
						case 2:
							file << "time";
							break;
						case 3:
							file << "compression\nratio";
							break;
					}
					continue;
				}
				switch (j) {
					case 0:
						file << getAlgoName(i);
						break;
					case 1:
						file << size << "B";
						break;
					case 2:
						file << dur << "ms";
						break;
					case 3:
						file << float(size) / float(std::filesystem::file_size(filename)) * 100 << "%";
				}
				file << "</th>\n";
			}
			file << "</tr>";
		}
		file << "</table>";
		file << "The algorithm achieving the highest compression ration is: " + getAlgoName(minsizealgo)
		     << "<br>The fastest algorithm is: " + getAlgoName(mintimealgo);
		file.close();

		std::filesystem::remove(output);

		std::cout << "Best compressing algorithm: " << getAlgoName(minsizealgo) << std::endl;
		std::cout << "Fastest algorithm: " << getAlgoName(mintimealgo) << std::endl;
	}
}