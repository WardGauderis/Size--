//============================================================================
// @name        : production.h
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <array>
#include "variable.h"

using Production = std::array<Variable, 2>;

template<>
struct std::hash<Production>
{
    std::size_t operator()(const Production& p) const noexcept
    {
        return (static_cast<uint64_t>(p[0]) << 32u) + p[1];
    }
};