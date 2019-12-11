//============================================================================
// @name        : huffman.h
// @author      : Thomas Dooms
// @date        : 11/12/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>
#include <functional>

#include "../util/variable.h"
#include "../util/production.h"
#include "../pal/metadata.h"


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

    using EncodingTable = std::vector<std::pair<uint32_t, uint8_t>>;

    std::vector<uint32_t> countFrequencies(const std::vector<Variable>& string, const std::vector<Production>& productions, pal::Metadata metadata);
    std::unique_ptr<Node> createHuffmanTree(const std::vector<uint32_t>& frequencies);
    EncodingTable createEncodingTable(const std::unique_ptr<Node>& root, pal::Metadata metadata);

    void printEncodingTable(const EncodingTable& table);
    void dotHuffmanTree(const std::string& path, const std::unique_ptr<Node>& root);
}





















