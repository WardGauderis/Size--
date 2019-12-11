//============================================================================
// @name        : type.h
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#pragma once

#include <istream>

enum class Algorithm {
	none,
	bisection,
	bisectionPlusPlus,
	repair,
	sequitur,
	sequential,
	lzw,
};

enum class Mode
{
    none_specified,
    memory_efficient,
    average,
    fast,
};

std::istream& operator>>(std::istream& in, Algorithm& algorithm);

std::istream& operator>>(std::istream& in, Mode& mode);

std::ostream& operator<<(std::ostream& os, const Mode& mode);