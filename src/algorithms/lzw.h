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
#include "../pal/onlineReader.h"
#include "../pal/onlineWriter.h"

namespace algorithm::lzw
{
    void decompress(const std::vector<Variable> &variables,
                    const std::experimental::filesystem::path &output)
    {

        std::ofstream outputfile{output};
        if(not outputfile.is_open()) throw std::runtime_error("could not open file: " + output.string());

        robin_hood::unordered_flat_map <uint32_t , std::string > map;
        for (int i = 0; i <= 255; i++) map[i] = char(i);

        uint32_t old = variables[0], index;
        std::string s = map[old], c;
        c += s[0];
        outputfile << s;
        uint32_t val = 256;
        for (uint i = 0; i < variables.size() - 1; i++) {

            index = variables[i + 1];
            if (map.find(index) == map.end()) s = map[old] + c;
            else s = map[index];

            outputfile << s;
            c = s[0];
            map[val] = map[old] + c;
            val++;
            old = index;
        }
        outputfile.close();
    }

    std::tuple<Settings, std::vector<Variable>, std::vector<Production>> compress(
            const std::experimental::filesystem::path& input)
    {
        // prepare output
        Settings settings;
        std::vector<Variable> variables;
        variables.reserve(100000);

        robin_hood::unordered_flat_map <std::string , uint32_t > map;
        for (int i=0;i<256;i++)
        {
            std::string t;
            t += static_cast<char>(i);
            map [t] = i;
        }

        OnlineReader reader(input);
        auto size = reader.getSize();
        char c =reader.readVariable();

        uint32_t value = 256;
        std::string p; p+=c;
        //compress
        for (size_t i=1; i < size; ++i)
        {
            c = reader.readVariable();
            auto it = map.find(p + c);
            if (it != map.end()) p += c;
            else {
                variables.emplace_back(map.at(p));
                map[p+c] = value;
                value++;
                p = c;
            }
        }
        variables.emplace_back(map.at(p));
        //decompress(variables, std::experimental::filesystem::path("./test.bmp"));
        return std::make_tuple(settings, std::move(variables), std::vector<Production>{});
    }
}