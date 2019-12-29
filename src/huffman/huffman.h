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
#include "node.h"


namespace huffman
{
    using EncodingTable = std::vector<std::pair<uint32_t, uint8_t>>;

    std::vector<uint32_t> countFrequencies(const std::vector<Variable>& string, const std::vector<Production>& productions, pal::Metadata metadata);
    std::unique_ptr<Node> createHuffmanTree(const std::vector<uint32_t>& frequencies);
    EncodingTable createEncodingTable(const std::unique_ptr<Node>& root, pal::Metadata metadata);
}





















