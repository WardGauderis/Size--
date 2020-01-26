//============================================================================
// @name        : bitwriter.h
// @author      : Thomas Dooms
// @date        : 11/13/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <fstream>
#include <bitset>
#include <iostream>
#include <filesystem>

class Bitwriter
{
public:
    explicit Bitwriter(const std::filesystem::path& path)
    {
        file = std::ofstream(path, std::ios::binary);
        buffer = 0;
        index = 0;
    }
    ~Bitwriter()
    {
        if(index != 0) write_buffer();
    }

    void write_unordered_byte(uint8_t byte)
    {
        file.write(reinterpret_cast<char*>(&byte), sizeof(uint8_t));
    }

    void write_unordered_string(const std::string& str)
    {
        file.write(str.data(), str.size());
    }

    void write_bit(bool bit) noexcept
    {
        if(index == 64)
        {
            write_buffer();
            buffer = bit;
            buffer <<= 63u;
            index = 1;
        }
        else
        {
            index++;
            buffer += static_cast<uint64_t>(bit) << (buffer_size - index);
        }
    }

    template<typename Type>
    void write_value(Type value, uint8_t size = sizeof(Type) * 8) noexcept
    {
        static_assert(std::is_integral_v<Type> and std::is_unsigned_v<Type>, "written type must be an unsigned integral");
        const auto shift = buffer_size - index;
        if(index == 64)
        {
            write_buffer();
            buffer = static_cast<uint64_t>(value) << (buffer_size - size);
            index = size;
        }
        else if(index + size > buffer_size)
        {
            // shift the top bits of number into buffer
            buffer += static_cast<uint64_t>(value) >> (size - shift);

            // write to buffer
            write_buffer();

            //reset buffer to num
            buffer = static_cast<uint64_t>(value) << (buffer_size - size + shift);

            // set the index
            index = size - shift;
        }
        else
        {

            buffer += static_cast<uint64_t>(value) << (shift - size);
            index += size;
        }
    }

    void write_buffer() noexcept
    {
        file.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
    }

    auto& get_file()
    {
        return file;
    }

    size_t getCurrentPos()
    {
        return file.tellp();
    }

private:

    std::ofstream file;
    uint64_t buffer;
    uint64_t index;

    constexpr inline static auto buffer_size = sizeof(buffer) * 8;
};


