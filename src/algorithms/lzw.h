// =====================================================================
// @name: lzw.h
// @project: SIZE--
// @author: Mano Marichal
// @date: 25.11.19
// @copyright: BA2 Informatica - Mano Marichal - University of Antwerp
// @description:
// =====================================================================

#pragma once
#include <vector>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <experimental/filesystem>
#include <tuple>
#include <fstream>

#include "../util/robin_hood.h"
#include "../util/variable.h"
#include "../util/production.h"
#include "../util/settings.h"
#include "../util/settings.h"
#include "../bitio/bitreader.h"
#include "../bitio/bitwriter.h"
#include "../pal/metadata.h"
#include "../pal/onlineReader.h"
#include "../pal/onlineWriter.h"

namespace algorithm::lzw
{

    std::tuple<Settings, std::vector<Variable>, std::vector<Production>> compress(
            const std::experimental::filesystem::path& input)
    {
        using Pair = uint32_t;
        constexpr auto compose   = [](Variable lhs, Variable rhs) -> Pair { return (static_cast<Pair>(lhs) << 16u) + rhs; };
        // read input
        std::ifstream inputFile (input);
        if(not inputFile.is_open()) throw std::runtime_error("could not open file: " + input.string());
        char c;
        inputFile.get(c);

        // prepare outputfile
        Settings settings;

        std::vector<Variable> variables;
        //robin_hood::unordered_flat_map <uint32_t , uint16_t> map;
        std::unordered_map<std::string , uint32_t > map;
        for (int i=0;i<256;i++)
        {
            std::string t;
            t += static_cast<char>(i);
            map [t] = i;
        }

        uint16_t value = 256;
        std::string p = ""; p+=c;

        //compress
        while (inputFile.get(c))
        {
            auto it = map.find(p + c);
            if (it != map.end())
            {
                p += c;
            }
            else {
                variables.emplace_back(map.at(p));
                map[p+c] = value;
                value++;
                p = c;
            }
        }
        variables.emplace_back(map.at(p));

        return std::make_tuple(settings, std::move(variables), std::vector<Production>{});
    }

    std::pair<std::vector<Production>, std::vector<Variable>> decompress(std::vector<Variable> variables)
    {
        std::unordered_map<std::string , uint32_t > map;
        for (int i=0;i<256;i++)
        {
            std::string t;
            t += static_cast<char>(i);
            map [t] = i;
        }
    }
}
