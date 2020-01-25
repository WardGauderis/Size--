//
// Created by ward on 11/23/19.
//

#include <iostream>
#include "controller.h"
#include "pal.h"

using namespace boost::program_options;

Controller::Controller(int argc, char** argv) : desc(options_description("options")) {
	desc.add_options()
			("help,h", "produce this message")
			("verbose,v", "be verbose")
			("visualize,V", "visualize the compression algorithm in dot (only applicable to the compression of small files)")
			("output,o", value<std::filesystem::path>(), "output file/directory (optional)")
			("create,c", value<Algorithm>(), "create a new archive with the specified algorithm (index or name):\n"
			                                 "0 none\n"
			                                 "1 bisection\n"
			                                 "2 bisection++\n"
			                                 "3 LCA\n"
			                                 "4 OLCA\n"
			                                 "5 repair *\n"
			                                 "6 sequitur\n"
			                                 "7 sequential\n"
			                                 "8 LZW")
			("mode,m", value<Mode>()->default_value(Mode::none_specified),
			 "run the algorithm in a certain mode (index or name; optional; only applies to algorithms with a *):\n"
			 "0 none\n"
			 "1 memory\n"
			 "2 average\n"
			 "3 fast")
			("extract,x", "extract file(s) from an archive");

	options_description hidden;
	hidden.add_options()
			("files", value<std::vector<std::filesystem::path>>(&files), "input file(s)");

	options_description combined;
	combined.add(desc).add(hidden);

	positional_options_description pos;
	pos.add("files", -1);

	try {
		store(command_line_parser(argc, argv).options(combined).positional(pos).run(), vm);
		notify(vm);

		if (vm.count("help")) {
			printHelp();
			return;
		}

		checkOptions();

		execute();
	} catch (const error& ex) {
		returnValue += 1;
		std::cout << std::endl << ex.what() << "\n\n";
		printHelp();
		return;
	}

}

void Controller::checkOptions() {
	if (!(vm.count("create") ^ vm.count("extract"))) throw error("need to specify creation or extraction");
	if (!vm.count("files")) throw error("need at least one file to run on");
	if (files.size() > 1) {
		tar = true;
		if (vm.count("extract")) throw error("only one pal archive can be extracted");
	}
	for (const auto& file: files) {
		if (!std::filesystem::exists(file)) throw error("file '" + file.string() + "' is not found");
		if (std::filesystem::is_directory(file)) tar = true;
	}
}

void Controller::execute() {
	if (vm.count("create")) {
		compress();
	} else if (vm.count("extract")) {
		extract();
	}
}

int Controller::getReturnValue() const {
	return returnValue;
}

void Controller::printHelp() const {
	std::cout << "usage: SIZE-- [options] <files>\n";
	std::cout << desc;
}

void Controller::compress() {
	auto temp = std::filesystem::temp_directory_path() / "tempsizeminmin.tar";
	auto in = files[0];

	std::filesystem::path outputFile;
	std::filesystem::path outputDirectory;

	if (vm.count("output")) {
		auto out = vm["output"].as<std::filesystem::path>();
		if (std::filesystem::is_directory(out)) {
			outputDirectory = out;
			outputFile = std::filesystem::path(in.filename().string() + ".pal");
		} else {
			outputDirectory = out;
			outputDirectory.remove_filename();
			outputFile = out.filename();
		}
	} else {
		outputDirectory = ".";
		outputFile = std::filesystem::path(in.filename().string() + ".pal");
	}

	if (tar) {
		std::string command = "tar -cf " + temp.string();
		for (const auto& file: files) {
			command += " " + file.string();
		}
		command += " >/dev/null 2>&1";
		returnValue += system(command.c_str());
		in = temp.string();
	}

	pal::encode(in, outputDirectory / outputFile, vm["create"].as<Algorithm>(), vm["mode"].as<Mode>(), tar, vm.count("verbose"), vm.count("visualize"));
}

void Controller::extract() {
	auto temp = std::filesystem::temp_directory_path() / "tempsizeminmin.tar";
	auto in = files[0];

	std::filesystem::path outputFile;
	std::filesystem::path outputDirectory;

	if (vm.count("output")) {
		auto out = vm["output"].as<std::filesystem::path>();
		if (std::filesystem::is_directory(out)) {
			outputDirectory = out;
			outputFile = in.filename();
			if (outputFile.extension() == ".pal")
				outputFile.replace_extension("");
		} else {
			outputDirectory = out;
			outputDirectory.remove_filename();
			outputFile = out.filename();
		}
	} else {
		outputDirectory = ".";
		outputFile = in.filename();
		if (outputFile.extension() == ".pal")
			outputFile.replace_extension("");
	}

	tar = pal::decode(in, temp);

	if (tar) {
		std::string command = "tar -xf " + temp.string() + " -C " + outputDirectory.string() + " >/dev/null 2>&1";
		returnValue += system(command.c_str());
		return;
	}

	std::string command =
			"mv " + temp.string() + " " + (outputDirectory / outputFile).string() + " >/dev/null 2>&1";
	returnValue += system(command.c_str());
}

