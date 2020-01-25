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

void encode(const std::experimental::filesystem::path& input, const std::experimental::filesystem::path& output, Algorithm type, Mode mode, bool tar, bool verbose, bool visualize)
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
        else if(type == Algorithm::olca)
        {
        	OnlineReader reader(input);
	        return algorithm::olca::compress(reader);
        } else if(type == Algorithm::lca)
        {
        	auto variables = readVariables(input);
        	return algorithm::lca::compress(std::move(variables));
        } else if(type == Algorithm::lzw)
        {
            return algorithm::lzw::compress(input);
        }
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
    pal::Encoder::encode(output, string, productions, metadata, verbose, visualize);
}

bool decode(const std::experimental::filesystem::path& input, const std::experimental::filesystem::path& output)
{
    return Decoder::decode(input, output);
}

// ------------------------------------------------------- //

std::vector<uint8_t> readBytes(const std::experimental::filesystem::path& path)
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

std::pair<std::vector<Variable>, bool> readPairs(const std::experimental::filesystem::path& path)
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

std::vector<Variable> readVariables(const std::experimental::filesystem::path& path)
{
    const auto bytes = readBytes(path);
    std::vector<Variable> variables(bytes.begin(), bytes.end());
    return variables;
}

std::vector<Variable> readSmartVariables(const std::experimental::filesystem::path& path)
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



}
