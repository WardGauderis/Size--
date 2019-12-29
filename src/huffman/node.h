//============================================================================
// @name        : node.h
// @author      : Thomas Dooms
// @date        : 12/29/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <memory>

namespace huffman
{
    struct Node
    {
        Node()                       : character(0), frequency(0), left(nullptr), right(nullptr) {}
        Node(uint32_t c)             : character(c), frequency(0), left(nullptr), right(nullptr) {}
        Node(uint32_t c, uint32_t f) : character(c), frequency(f), left(nullptr), right(nullptr) {}

        [[nodiscard]] bool leaf() const noexcept { return character != std::numeric_limits<uint32_t>::max(); }

        uint32_t character;
        uint32_t frequency;

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
}

