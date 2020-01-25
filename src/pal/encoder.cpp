//============================================================================
// @name        : palEncoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include <unordered_set>
#include "encoder.h"
#include "../util/robin_hood.h"

namespace pal
{

void Encoder::encode(const std::filesystem::path& path, const std::vector<Variable>& string, const std::vector<Production>& productions, Metadata metadata, bool verbose, bool visualize)
{
    Bitwriter writer(path);
    encodeMetadata(writer, metadata);

    if(metadata.settings.is_lca_encoded())
    {
        if(verbose) std::cout << "encoding with lca special algorithm thing\n";
        encodeLcaTree(writer, string, productions, metadata);
    }
    else
    {
        huffman::Encoder encoder(string, productions, metadata);

        encodeHuffmanTree(writer, encoder, metadata);
        if(verbose) std::cout << "  - after huffman tree, the file is approx: " << writer.getCurrentPos() << " bytes\n";

        encodeProductions(writer, encoder, productions);
        if(verbose) std::cout << "  - after productions, the file is approx : " << writer.getCurrentPos() << " bytes\n";

        encodeString(writer, encoder, string);
        if(verbose) std::cout << "  - after root string, the file is approx : " << writer.getCurrentPos() << " bytes\n";

        if(visualize)
        {
            std::filesystem::create_directory("visuals");
            visualize::huffmanTree("visuals", "huffman", encoder.root);
            visualize::parseTree("visuals", "parse", string, productions, metadata);
        }
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

void Encoder::encodeLcaTree(Bitwriter& writer, const std::vector<Variable>& string, const std::vector<Production>& productions, Metadata metadata)
{
    if(string.size() != 1) throw std::logic_error("aiaiai Ward");

    if(metadata.settings.is_lca_encoded() and metadata.settings.has_reserved())
        throw std::logic_error("aiaiai Ward 2");

    std::unordered_map<Variable, Variable> dictionary;
    size_t current = 0;

    std::function<void(Variable)> recurse = [&](Variable variable)
    {
        if(Settings::is_byte(variable))
        {
            writer.write_bit(true);
            writer.write_value(variable, metadata.charLength);
        }
        else if(const auto iter = dictionary.find(variable); iter != dictionary.end())
        {
            writer.write_bit(true);
            writer.write_value(iter->second + metadata.settings.begin(), metadata.charLength);
        }
        else
        {
            const auto production = productions[variable - metadata.settings.begin()];
            recurse(production[0]);
            recurse(production[1]);

            writer.write_bit(false);
            dictionary.emplace(variable, current++);
        }
    };

    recurse(string[0]);
}

}