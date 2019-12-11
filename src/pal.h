//============================================================================
// @name        : compressed.h
// @author      : Thomas Dooms
// @date        : 11/14/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <vector>
#include <filesystem>

#include "util/settings.h"
#include "util/production.h"
#include "algorithms/type.h"
#include "util/variable.h"

namespace pal
{
    void encode(const std::filesystem::path& input, const std::filesystem::path& output, Algorithm type, Mode mode, bool tar = false);
    bool decode(const std::filesystem::path& input, const std::filesystem::path& output);

    std::vector<uint8_t> readBytes(const std::filesystem::path& path);
    std::pair<std::vector<Variable>, bool> readPairs(const std::filesystem::path& path);
    std::vector<Variable> readVariables(const std::filesystem::path& path);
    std::vector<Variable> readSmartVariables(const std::filesystem::path& path);

    void writeYield(std::ofstream& file, const std::vector<Variable>& string, const std::vector<Production>& productions, Settings settings);
}
