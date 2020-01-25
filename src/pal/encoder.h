//============================================================================
// @name        : palEncoder.h
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <vector>
#include "../bitio/bitwriter.h"
#include "../huffman/encoder.h"
#include "metadata.h"
#include <filesystem>

namespace pal
{
    struct Encoder
    {
        static void encode(const std::filesystem::path& path, const std::vector<Variable>& string, const std::vector<Production>& productions, Metadata metadata, bool verbose, bool visualize);

        static void encodeMetadata   (Bitwriter& writer, Metadata metadata);
        static void encodeHuffmanTree(Bitwriter& writer, const huffman::Encoder& encoder, Metadata metadata);

        static void encodeProductions(Bitwriter& writer, const huffman::Encoder& encoder, const std::vector<Production>& productions);

        static void encodeString     (Bitwriter& writer, const huffman::Encoder& encoder, const std::vector<Variable>& string);
    };
}




