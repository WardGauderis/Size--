//============================================================================
// @name        : onlineReader.h
// @author      : Thomas Dooms
// @date        : 12/28/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <fstream>
#include <filesystem>
#include "../util/variable.h"

class OnlineReader
{
public:
    explicit OnlineReader(const std::filesystem::path& path)
    {
        stream = std::ifstream(path);
    }

    Variable readVariable()
    {
        Variable res = 0;
        stream.read(reinterpret_cast<char*>(&res), sizeof(uint8_t));
        return res;
    }

    size_t getSize()
    {
        if(stream.tellg() != 0) throw std::runtime_error("getSize must be called before reading");
        stream.seekg(0, std::ios::end);
        const auto size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        return size;
    }

private:
    std::ifstream stream;
};