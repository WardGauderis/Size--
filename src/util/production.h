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

using Production = std::array<uint32_t, 2>;

template<>
struct std::hash<Production>
{
    std::size_t operator()(const Production& p) const noexcept
    {
        return (static_cast<uint64_t>(p[0]) << 32u) + p[1];
    }
};
