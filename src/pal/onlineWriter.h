//============================================================================
// @name        : onlineWriter.h
// @author      : Thomas Dooms
// @date        : 12/28/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include "../util/settings.h"
#include "../bitio/bitwriter.h"
#include "metadata.h"

class OnlineWriter
{
public:
    explicit OnlineWriter(const std::filesystem::path& path) : writer(path)
    {
        writeMetadata(pal::Metadata(0,0,Settings(0)));
    }

    template<typename Type>
    void writeValue(Type value, uint8_t size)
    {
        writer.write_value(value, size);
    }

    void writeMetadata(pal::Metadata metadata)
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
private:
    Bitwriter writer;
};