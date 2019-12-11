//============================================================================
// @name        : onlineEncoder.h
// @author      : Thomas Dooms
// @date        : 11/26/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <filesystem>
#include <fstream>
#include "metadata.h"

#include "../bitio/bitwriter.h"
#include "../bitio/bitreader.h"

#include "encoder.h"

namespace pal
{
class OnlineHelper
{
    friend class Bitreader;
    friend class Bitwriter;

    OnlineHelper(const std::filesystem::path& input, const std::filesystem::path& output) : reader(input), writer(output)
    {
        writeMetadata(Metadata(0,0,Settings(0)));
    }

    uint8_t readByte()
    {
        return reader.read_value<uint8_t>();
    }

    template<typename Type>
    void writeValue(Type value, uint8_t size)
    {
        writer.write_value(value, size);
    }

    void writeMetadata(Metadata metadata)
    {
        uint8_t unique = 93;

        auto& file = writer.get_file();
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&metadata.stringSize    ), sizeof(metadata.stringSize));
        file.write(reinterpret_cast<char*>(&metadata.productionSize), sizeof(metadata.productionSize));
        file.write(reinterpret_cast<char*>(&metadata.settings.flags), sizeof(metadata.settings.flags));
        file.write(reinterpret_cast<char*>(&unique), sizeof(uint8_t));
        file.seekp(std::ios_base::end);
    }

    Bitreader reader;
    Bitwriter writer;
};

}




