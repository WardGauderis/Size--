//============================================================================
// @name        : huffmanDecoder.h
// @author      : Thomas Dooms
// @date        : 11/15/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include "huffman.h"

namespace huffman
{
    struct Decoder
    {
        explicit Decoder(std::unique_ptr<Node> root) : root(std::move(root))
        {
//            dotHuffmanTree("visuals/decode", this->root);
        }

        Variable decodeVariable(Bitreader& reader) const noexcept
        {
            const Node* node = root.get();
            while(not node->leaf())
            {
                node = reader.read_bit() ? node->left.get() : node->right.get();
            }
            return node->character;
        }

        std::unique_ptr<Node> root;
    };
}

