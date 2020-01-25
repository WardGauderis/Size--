//============================================================================
// @name        : palDecoder.h
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once


#include "../bitio/bitreader.h"

#include "../util/variable.h"
#include "../util/settings.h"
#include "../huffman/decoder.h"
#include "metadata.h"

namespace pal
{

struct Decoder
{
    static std::tuple<Metadata, std::vector<Production>, std::vector<Variable>> decode(const std::filesystem::path& path);

    static Metadata decodeMetadata(Bitreader& reader);
    static std::unique_ptr<huffman::Node> decodeHuffmanTree(Bitreader& reader, Metadata metadata);

    static std::vector<Production> decodeProductions(Bitreader& reader, huffman::Decoder& decoder, Metadata metadata);

    static std::vector<Variable> decodeString(Bitreader& reader, huffman::Decoder& decoder, Metadata metadata);
};

}

