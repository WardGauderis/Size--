//============================================================================
// @name        : visualize.h
// @author      : Thomas Dooms
// @date        : 12/29/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <fstream>
#include <filesystem>

#include "../huffman/node.h"

void dotHuffmanTree(const std::filesystem::path& directory, const std::string& name, const std::unique_ptr<Node>& root)
{
    if(not std::filesystem::is_directory(directory)) throw std::logic_error("directory path must be directory");

    const auto dot = directory / (name + ".dot");
    const auto png = directory / (name + ".png");

    std::ofstream file(dot);
    file << "digraph G{";

    uint32_t curr = 10000000;

    std::function<void(const std::unique_ptr<Node>&, uint32_t)> recursion = [&](const auto& root, const auto num) -> void
    {
        if(root == nullptr) return;
        uint32_t next;

        if(root->character == std::numeric_limits<uint32_t>::max())
        {
            file << curr << " [label=""];\n";
            file << num << " -> " << curr++ << '\n';
            next = curr;
        }
        else
        {
            file << num << " <- " << root->character << '\n';
            next = root->character;
        }

        if (root->left  != nullptr) recursion(root->left , next);
        if (root->right != nullptr) recursion(root->right, next);
    };

    recursion(root, std::numeric_limits<uint32_t>::max());

    file << '}';
    file << std::flush;

    system(("dot -Tpng " + dot.string() + " -o " + png.string() + "&").c_str());
}