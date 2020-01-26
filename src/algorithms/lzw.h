//============================================================================
// @name        : thomaslzw.h
// @author      : Thomas Dooms
// @date        : 1/26/20
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include "../pal/onlineReader.h"
#include "../pal/onlineWriter.h"
#include "../util/robin_hood.h"

#include <cstdio>

namespace algorithm::lzw
{
    void compress(const std::filesystem::path& input, const std::filesystem::path& output)
    {
        Settings settings(Settings::Flags::lzw);
        pal::Metadata metadata(0u, 0u, settings);
        robin_hood::unordered_flat_map<std::string, Variable> dict;

        {
            OnlineReader reader(input);
            OnlineWriter writer(output, metadata);

            const auto size = reader.getSize();
            std::string old_str;
            std::string str;
            Variable current = 256;

            for (Variable i = 0; i < current; i++)
            {
                dict.emplace(std::string(1, static_cast<char>(i)), i);
            }

            for (size_t i = 0; i < size; i++)
            {
                const char var = reader.readByte();
                const auto iter = dict.find(str + var);
                if (iter == dict.end())
                {
                    const auto charLength = std::floor(std::log2(dict.size())) + 1;
                    writer.writeValue(dict.at(str), charLength);

                    dict.emplace(str + var, current++);
                    str = var;
                }
                else
                {
                    str += var;
                }
            }
            const auto charLength = std::floor(std::log2(current)) + 1;
            writer.writeValue(dict.at(str), charLength);
        }
        // add one, to represent the last value.
        uint32_t size = (dict.size() + 1) - 256;
        auto* file = fopen(output.c_str(), "r+b");

        fwrite(&metadata.productionSize, sizeof(metadata.productionSize), 1, file);
        fwrite(&size, sizeof(metadata.productionSize), 1, file);
    }
}