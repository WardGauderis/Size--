//============================================================================
// @name        : compressed.cpp
// @author      : Thomas Dooms
// @date        : 11/14/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include <numeric>

#include "pal.h"

#include "pal/decoder.h"
#include "pal/encoder.h"
#include "algorithms/bisection.h"
#include "algorithms/repair.h"
#include "algorithms/lzw.h"
#include "algorithms/lca.h"
#include "algorithms/none.h"
#include "algorithms/sequitur.h"
#include "algorithms/olca.h"

namespace pal
{

void encode(const std::filesystem::path& input, const std::filesystem::path& output, Algorithm type, Mode mode, bool tar)
{
    auto [settings, string, productions] = [&]()
    {
        if(type == Algorithm::none)
        {
            const auto bytes = readBytes(input);
            return algorithm::none::compress(bytes);
        }
        else if(type == Algorithm::sequitur)
        {
            const auto bytes = readBytes(input);
            return algorithm::sequitur::compress(bytes);
        }
        else if(type == Algorithm::bisection)
        {
	        auto [variables, odd] = readPairs(input);
	        return algorithm::bisection::compress(std::move(variables), odd);
        }
        else if(type == Algorithm::bisectionPlusPlus)
        {
	        auto [variables, odd] = readPairs(input);
	        return algorithm::bisectionPlusPlus::compress(std::move(variables), odd);
        }
        else if(type == Algorithm::lca)
        {
	        auto [variables, odd] = readPairs(input);
//	        return algorithm::lca::compress(std::move(variables), odd);
        }
        else if(type == Algorithm::olca)
        {
        	OnlineReader reader(input);
	        return algorithm::olca::compress(reader);
        }
//        else if(type == Algorithm::lzw)
//        {
//            algorithm::lzw::compress(input, output);
//            exit(0);
//        }
        else if(type == Algorithm::repair)
        {
            auto variables = readVariables(input);
            return algorithm::repair::compress(std::move(variables), mode);
        }
        else
        {
            throw std::runtime_error("unknown algorithm");
        }
    }();

    if(tar) settings.flags |= Settings::Flags::tar;

    Metadata metadata(string.size(), productions.size(), settings);
    pal::Encoder::encode(output, string, productions, metadata);
}

bool decode(const std::filesystem::path& input, const std::filesystem::path& output)
{
    const auto [metadata, productions, string] = Decoder::decode(input);

    std::ofstream file(output, std::ios::binary);
    if(not file.is_open()) throw std::runtime_error("could not open file: " + output.string());
    writeYield(file, string, productions, metadata.settings);
    return metadata.settings.is_tar();
}

// ------------------------------------------------------- //

std::vector<uint8_t> readBytes(const std::filesystem::path& path)
{
    auto file = fopen(path.c_str(), "rb");
    if(not file) throw std::runtime_error("could not open file: " + path.string());
    fseek(file, 0, SEEK_END);
    const auto size = static_cast<size_t>(ftell(file));
    fseek(file, 0, SEEK_SET);

    std::vector<uint8_t> string(size);
    fread(string.data(), 1, size, file);

    fclose(file);
    return string;
}

std::pair<std::vector<Variable>, bool> readPairs(const std::filesystem::path& path)
{
    auto file = fopen(path.c_str(), "rb");
    if(not file) throw std::runtime_error("could not open file: " + path.string());
    fseek(file, 0, SEEK_END);
    const auto size = static_cast<size_t>(ftell(file));
    fseek(file, 0, SEEK_SET);

    std::vector<uint16_t> string((size / 2) + (size % 2));
    fread(string.data(), 2, size, file);

    fclose(file);
	std::vector<Variable> variables(string.begin(), string.end());

    return std::make_pair(std::move(variables), size % 2);
}

std::vector<Variable> readVariables(const std::filesystem::path& path)
{
    const auto bytes = readBytes(path);
    std::vector<Variable> variables(bytes.begin(), bytes.end());
    return variables;
}

std::vector<Variable> readSmartVariables(const std::filesystem::path& path)
{
    constexpr size_t swap_size = 2000000; // 20 mb
    auto file = fopen(path.c_str(), "rb");
    if(not file) throw std::runtime_error("could not open file: " + path.string());

    fseek(file, 0, SEEK_END);
    const auto size = static_cast<size_t>(ftell(file));
    fseek(file, 0, SEEK_SET);

    // it's important to make this in the function itself, and not in a branch to allow NRVO
    std::vector<Variable> result;

    if(size > swap_size)
    {
        std::vector<uint16_t> temp((size / 2) + (size % 2));
        fread(temp.data(), 2, size, file);
        fclose(file);

        result.assign(temp.begin(), temp.end());

        std::for_each(result.begin(), result.end() - 1, [](auto& elem) { elem += 256; });
        if (not size % 2) result.back() += 256;
    }
    else
    {
        std::vector<uint8_t> temp(size);
        fread(temp.data(), 1, size, file);
        fclose(file);

        result.assign(temp.begin(), temp.end());
    }

    return result;
}

void writeYield(std::ofstream& file, const std::vector<Variable>& string, const std::vector<Production>& productions, Settings settings)
{
    std::vector<std::string> yields(productions.size());

    const auto calcSize = [&](auto base, auto index) -> size_t
    {
        if (Settings::is_byte(index)) return base + 1;
        else if (settings.is_reserved_variable(index)) return base + 2;
        else return base + yields[index - settings.begin()].size();
    };

    for(size_t i = 0; i < productions.size(); i++)
    {
        const auto evaluate = [&](auto index)
        {
            if(Settings::is_byte(index))
            {
                yields[i] += index;
            }
            else if(settings.is_reserved_variable(index))
            {
                const auto [var0, var1] = Settings::convert_from_reserved(index);
                yields[i] += var0;
                yields[i] += var1;
            }
            else
            {
                yields[i] += yields[index - settings.begin()];
            }
        };

        yields[i].reserve(calcSize(calcSize(0, productions[i][0]), productions[i][1]));
        evaluate(productions[i][0]);
        evaluate(productions[i][1]);
    }

    for(const auto index : string)
    {
        if(Settings::is_byte(index))
        {
            file.write(reinterpret_cast<const char*>(&index), 1);
        }
        else if(settings.is_reserved_variable(index))
        {
            const auto [var0, var1] = Settings::convert_from_reserved(index);
            file.write(reinterpret_cast<const char*>(&var0), 1);
            file.write(reinterpret_cast<const char*>(&var1), 1);
        }
        else
        {
            const auto& yield = yields[index - settings.begin()];
            file.write(yield.data(), yield.size());
        }
    }
}

}
