// =====================================================================
// @name: sequitur.h
// @project: SIZE--
// @author: Mano Marichal
// @date: 18.11.19
// @copyright: BA2 Informatica - Mano Marichal - University of Antwerp
// @description: 
// =====================================================================

#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <list>
#include <fstream>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <tuple>

#include "../util/production.h"
#include "../util/settings.h"

namespace algorithm::sequitur
{
    using Digram = std::pair<uint32_t, uint32_t>;
    using limits = std::numeric_limits<uint32_t>;

    struct Node
    {
        uint32_t value;
        Node* next = nullptr;
        Node* previous = nullptr;
        Node(uint32_t value, Node* next, Node* previous): value(value), next(next), previous(previous) {};
        Node(uint32_t value): value(value) {};
    };

    class Encoder
    {
        std::vector<Node> nodes;
        std::vector<Production> productions;
        std::unordered_map<Digram, Node *, boost::hash<Digram>> index;
        std::unordered_map<Digram, std::pair<uint32_t, uint32_t>, boost::hash<Digram>> rules;
        uint32_t begin;
        Node* head;

        // erases a node from the linked chain
        void erase(Node * node);
        // function that looks at the digram the node forms and takes actions accordingly
        void linkSymbol(Node* &node);
        // replaces a digram by a rule
        void replaceByRule(Node* node, uint32_t rule);
        // converts the initial vector of characters in to nodes
        static std::vector<uint32_t> NodesToVector(Node * start);
        // decode function
        void decode(uint32_t c, uint32_t begin);

    public:
        std::tuple<Settings, std::vector<Variable>, std::vector<Production>> compress(std::vector<unsigned char> string);

    };

    std::tuple<Settings, std::vector<Variable>, std::vector<Production>> compress(std::vector<unsigned char> string);

    // std::pair<std::vector<Variable>, std::vector<Production>>
}
