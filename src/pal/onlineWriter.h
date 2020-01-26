//============================================================================
// @name        : onlineWriter.h
// @author      : Thomas Dooms
// @date        : 12/28/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include "../bitio/bitwriter.h"
#include "metadata.h"

class OnlineWriter
{
public:
    explicit OnlineWriter(const std::filesystem::path& path, pal::Metadata metadata) : writer(path)
    {
        if(not std::filesystem::exists(path)) throw std::runtime_error("deze file bestaat niet mano");
        writer.write_value(metadata.stringSize, 32u);
        writer.write_value(metadata.productionSize, 32u);
        writer.write_value(metadata.settings.flags, 8u);
        writer.write_value(93u, 8u);
    }

    template<typename Type>
    void writeValue(Type value, uint8_t size)
    {
        writer.write_value(value, size);
    }
private:
    Bitwriter writer;
};
