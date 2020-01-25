#include "controller.h"
#include <iostream>

int main(int argc, char** argv)
{
	try {
		Controller controller(argc, argv);
		return controller.getReturnValue();
	} catch (const std::exception& ex){
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}