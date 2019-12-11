//============================================================================
// @name        : palEncoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include "encoder.h"

namespace pal
{

void Encoder::encode(const std::filesystem::path& path, const std::vector<Variable>& string, const std::vector<Production>& productions, Metadata metadata)
{
    huffman::Encoder encoder(string, productions, metadata);
    Bitwriter writer(path);

    encodeMetadata(writer, metadata);

    if(metadata.settings.is_nohuffman())
    {
        encodeWithoutHuffman(writer, productions, string, metadata);
    }
    else
    {
        encodeHuffmanTree(writer, encoder, metadata);
        encodeProductions(writer, encoder, productions);
        encodeString     (writer, encoder, string);
    }
}

void Encoder::encodeMetadata(Bitwriter& writer, Metadata metadata)
{
    writer.write_value(metadata.stringSize);
    writer.write_value(metadata.productionSize);
    writer.write_value(metadata.settings.flags);
    writer.write_value(static_cast<uint8_t>(93u));
}

void Encoder::encodeHuffmanTree(Bitwriter& writer, const huffman::Encoder& encoder, Metadata metadata)
{
    const std::function<void(Bitwriter&, const huffman::Node*, uint32_t)> recursion = [&](auto& writer, const auto* root, auto charLength) -> void
    {
        if(root->leaf())
        {
            writer.write_bit(true);
            writer.write_value(root->character, charLength);
        }
        else
        {
            writer.write_bit(false);
            recursion(writer, root->left.get() , charLength);
            recursion(writer, root->right.get(), charLength);
        }
    };

    recursion(writer, encoder.root.get(), metadata.charLength);
}

void Encoder::encodeProductions(Bitwriter& writer, const huffman::Encoder& encoder, const std::vector<Production>& productions)
{
    for(auto production : productions)
    {
        encoder.encodeVariable(writer, production[0]);
        encoder.encodeVariable(writer, production[1]);
    }
}

void Encoder::encodeString(Bitwriter& writer, const huffman::Encoder& encoder, const std::vector<Variable>& string)
{
    for(const auto variable : string)
    {
        encoder.encodeVariable(writer, variable);
    }
}

void Encoder::encodeWithoutHuffman(Bitwriter& writer, const std::vector<Production>& productions, const std::vector<Variable>& string, Metadata metadata)
{
    const auto size = metadata.charLength;

    for(const auto production : productions)
    {
        writer.write_value(production[0], size);
        writer.write_value(production[1], size);
    }
    for(const auto variable : string)
    {
        writer.write_value(variable, size);
    }
}

}