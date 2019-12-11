//============================================================================
// @name        : metadata.h
// @author      : Thomas Dooms
// @date        : 11/18/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include "../util/settings.h"

namespace pal
{
class Metadata
{
public:
    Metadata(uint32_t stringSize, uint32_t productionSize, Settings settings) : stringSize(stringSize), productionSize(productionSize), settings(settings)
    {
        charLength = std::floor(std::log2(settings.begin() + productionSize)) + 1;
    }

    uint32_t stringSize;
    uint32_t productionSize;
    uint8_t charLength;
    Settings settings;
};
}