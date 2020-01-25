//============================================================================
// @name        : palDecoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include "decoder.h"

namespace pal
{

std::tuple<Metadata, std::vector<Production>, std::vector<Variable>> Decoder::decode(const std::filesystem::path& path)
{
    Bitreader reader(path);

    const auto metadata = decodeMetadata(reader);
    auto root = decodeHuffmanTree(reader, metadata);
    huffman::Decoder decoder(std::move(root));

    auto productions = decodeProductions(reader, decoder, metadata);
    auto string = decodeString(reader, decoder, metadata);

    return std::make_tuple(metadata, std::move(productions), std::move(string));
}

Metadata Decoder::decodeMetadata(Bitreader& reader)
{
    const auto stringSize = reader.read_value<uint32_t>();
    const auto productionSize = reader.read_value<uint32_t>();
    const auto flags = reader.read_value<uint8_t>();
    const auto code = reader.read_value<uint8_t>();
    if(code != 93u) throw std::runtime_error("file is not .pal or is corrupted");

    return Metadata(stringSize, productionSize, Settings(flags));
}

std::unique_ptr<huffman::Node> Decoder::decodeHuffmanTree(Bitreader& reader, Metadata metadata)
{
    if(reader.read_bit())
    {
        const auto character = reader.read_value(metadata.charLength);
        return std::make_unique<huffman::Node>(character);
    }
    else
    {
        auto node = std::make_unique<huffman::Node>();
        node->left  = decodeHuffmanTree(reader, metadata);
        node->right = decodeHuffmanTree(reader, metadata);
        node->character = std::numeric_limits<uint32_t>::max();
        return node;
    }
}

std::vector<Production> Decoder::decodeProductions(Bitreader& reader, huffman::Decoder& decoder, Metadata metadata)
{
    std::vector<Production> result(metadata.productionSize);
    for(auto& production : result)
    {
        production[0] = decoder.decodeVariable(reader);
        production[1] = decoder.decodeVariable(reader);
    }
    return result;
}

std::vector<Variable> Decoder::decodeString(Bitreader& reader, huffman::Decoder& decoder, Metadata metadata)
{
    std::vector<Variable> result(metadata.stringSize);
    for(size_t i = 0; i < metadata.stringSize; i++)
    {
        result[i] = decoder.decodeVariable(reader);
    }
    return result;
}

}