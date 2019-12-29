//
// Created by ward on 11/23/19.
//

#ifndef SIZE_CONTROLLER_H
#define SIZE_CONTROLLER_H

#include <boost/program_options.hpp>
#include <filesystem>
class Controller {
public:
	Controller(int argc, char** argv);

	[[nodiscard]] int getReturnValue() const;

private:
	boost::program_options::variables_map vm;
	boost::program_options::options_description desc;
	std::vector<std::filesystem::path> files;
	bool tar = false;
	int returnValue = 0;

	void checkOptions();

	void execute();

	void compress();

	void extract();

	void printHelp() const;
};


#endif //SIZE_CONTROLLER_H
