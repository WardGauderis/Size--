//============================================================================
// @name        : encodeMetadata.h
// @author      : Thomas Dooms
// @date        : 11/15/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <bitset>
#include <cmath>
#include <array>
#include <iostream>

struct Settings
{
    enum Flags : size_t
    {
        no_flags = 0u,
        reserved = 1u,
        no_huffman = 2u,
        tar = 4u,
    };

    explicit Settings(uint8_t flags = Flags::reserved) : flags(flags) {}

    [[nodiscard]] static uint32_t convert_to_reserved(uint32_t first, uint32_t second)
    {
        return byte_end + (second << 8u) + first;
    }
    [[nodiscard]] static std::array<uint32_t, 2> convert_from_reserved(uint32_t index)
    {
        const auto temp = index - byte_end;
        return {temp & 0x000000FFu, temp >> 8u};
    }

    [[nodiscard]] bool static is_reserved_rule(uint32_t first, uint32_t second)
    {
        return first < byte_end and second < byte_end;
    }

    [[nodiscard]] uint32_t offset(uint32_t index) const noexcept
    {
        return begin() + index;
    }
    [[nodiscard]] uint32_t begin() const noexcept
    {
        return (flags & Flags::reserved) ? reserved_end : byte_end;
    }
    [[nodiscard]] static uint32_t end() noexcept
    {
        return std::numeric_limits<uint32_t>::max();
    }

    [[nodiscard]] static bool is_byte(uint32_t index)
    {
        return index < byte_end;
    }
    [[nodiscard]] bool is_reserved_variable(uint32_t index)
    {
        return has_reserved() and index >= byte_end and index < reserved_end;
    }
    [[nodiscard]] bool is_valid(uint32_t index)
    {
        return index >= begin() and index < end();
    }
    [[nodiscard]] bool has_reserved() const noexcept
    {
        return flags & reserved;
    }
    [[nodiscard]] bool is_tar() const noexcept
    {
        return flags & tar;
    }
    [[nodiscard]] bool is_nohuffman() const noexcept
    {
        return flags & no_huffman;
    }

    uint8_t flags;

    constexpr inline static uint32_t byte_end = 256;
    constexpr inline static uint32_t reserved_end = 256 + 256 * 256;
};