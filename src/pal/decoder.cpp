//============================================================================
// @name        : palDecoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include <stack>
#include "../util/robin_hood.h"
#include "decoder.h"

namespace pal
{

bool Decoder::decode(const std::filesystem::path& input, const std::filesystem::path& output)
{
    Bitreader reader(input);
    Bitwriter writer(output);

    const auto metadata = decodeMetadata(reader);
    if(metadata.settings.is_lca_encoded())
    {
        writeDecodedLcaYield(writer, reader, metadata);
    }
    else if(metadata.settings.is_lzw_compressed())
    {
        writeDecodedLzwYield(writer, reader, metadata);
    }
    else
    {
        auto root = decodeHuffmanTree(reader, metadata);
        huffman::Decoder decoder(std::move(root));

        auto productions = decodeProductions(reader, decoder, metadata);
        auto string = decodeString(reader, decoder, metadata);
        writeDecodedYield(writer, string, productions, metadata);
    }
    return metadata.settings.is_tar();
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

void Decoder::writeDecodedLzwYield(Bitwriter& writer, Bitreader& reader, Metadata metadata)
{
    robin_hood::unordered_map<Variable, std::string> dict;
    Variable current = 256;

    for(Variable i = 0; i < current; i++)
    {
        dict.emplace(i, std::string(1, static_cast<char>(i)));
    }

    Variable old = reader.read_value(std::floor(std::log2(dict.size())) + 1), index;
    std::string str = dict[old];
    writer.write_unordered_string(str);

    char curr_char = str[0];

    for (size_t i = 1; i < metadata.stringSize; i++)
    {
        const auto charLength = std::floor(std::log2(dict.size() + 1)) + 1;
        index = reader.read_value(charLength);

        if (dict.find(index) == dict.end()) str = dict[old] + curr_char;
        else str = dict[index];

        writer.write_unordered_string(str);
        curr_char = str[0];
        dict[current] = dict[old] + curr_char;
        current++;
        old = index;
    }
}

void Decoder::writeDecodedLcaYield(Bitwriter& writer, Bitreader& reader, Metadata metadata)
{
    if(metadata.settings.is_lca_encoded() and metadata.settings.has_reserved())
        throw std::logic_error("cannot be lca encoded and reserved at the same time");

    std::vector<Production> dictionary;
    std::stack<Variable> stack;

    for(size_t i = 0; i < (2 * metadata.productionSize + 1); i++)
    {
        const bool bit = reader.read_bit();
        if(bit)
        {
            const Variable variable = reader.read_value(metadata.charLength);
            writeRecursiveVariable(writer, variable, dictionary, metadata);
            stack.push(variable);
        }
        else
        {
            const auto v2 = stack.top();
            stack.pop();
            const auto v1 = stack.top();
            stack.top() = metadata.settings.begin() + dictionary.size();
            // instead of popping we replace

            Production p = {v1, v2};
            dictionary.emplace_back(p);
        }
    }
}

void Decoder::writeDecodedYield(Bitwriter& writer, const std::vector<Variable>& string, const std::vector<Production>& productions, Metadata metadata)
{
    for(const auto variable : string)
    {
        writeRecursiveVariable(writer, variable, productions, metadata);
    }
}

void Decoder::writeRecursiveVariable(Bitwriter& writer, Variable variable, const std::vector<Production>& productions, Metadata metadata)
{
    if(Settings::is_byte(variable))
    {
        writer.write_unordered_byte(variable);
    }
    else if(metadata.settings.is_reserved_variable(variable))
    {
        const auto [var0, var1] = Settings::convert_from_reserved(variable);
        writer.write_unordered_byte(var0);
        writer.write_unordered_byte(var1);
    }
    else
    {
        const auto production = productions[variable - metadata.settings.begin()];
        writeRecursiveVariable(writer, production[0], productions, metadata);
        writeRecursiveVariable(writer, production[1], productions, metadata);
    }
}

}