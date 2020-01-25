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
#include "../util/production.h"
#include "../util/variable.h"
#include "../pal/metadata.h"

namespace visualize
{
    void huffmanTree(const std::filesystem::path& directory, const std::string& name, const std::unique_ptr<huffman::Node>& root);

    void parseTree(const std::filesystem::path& directory, const std::string& name, const std::vector<Variable>& string, const std::vector<Production>& productions, pal::Metadata metadata);
}
