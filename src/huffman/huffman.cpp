//============================================================================
// @name        : huffman.cpp
// @author      : Thomas Dooms
// @date        : 11/12/19
// @version     :
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description :
//============================================================================

#include "huffman.h"

namespace huffman
{

std::vector<uint32_t> countFrequencies(const std::vector<Variable>& string, const std::vector<Production>& productions, pal::Metadata metadata)
{
    const auto size = metadata.settings.offset(metadata.productionSize);
    std::vector<uint32_t> frequencies(size);

    std::for_each(string.begin(), string.end(), [&](const auto elem){ frequencies[elem]++; });

    for(const auto production : productions)
    {
        frequencies[production[0]]++;
        frequencies[production[1]]++;
    }

    return frequencies;
}

std::unique_ptr<Node> createHuffmanTree(const std::vector<uint32_t>& frequencies)
{
    constexpr auto compare = [](const std::unique_ptr<Node>& lhs, const std::unique_ptr<Node>& rhs)
    {
        return lhs->frequency > rhs->frequency;
    };

    std::vector<std::unique_ptr<Node>> min_heap(frequencies.size());

    // put all the data into data structure that will be heapified.
    auto num_elements = 0;
    for(size_t i = 0; i < frequencies.size(); i++)
    {
        if(frequencies[i] == 0) continue;
        else min_heap[num_elements++] = std::make_unique<Node>(i, frequencies[i]);
    }

    // we need to have an exact size for the heap operations
    min_heap.resize(num_elements);

    // make vector into a heap, to efficiently remove smallest elements
    std::make_heap(min_heap.begin(), min_heap.end(), compare);

    // continue popping heap until there is one element left
    while (min_heap.size() != 1)
    {
        // create new node
        auto top = std::make_unique<Node>();

        // pop first min value and put in left subtree
        std::swap(min_heap.front(), top->left);
        std::pop_heap(min_heap.begin(), min_heap.end(), compare);
        min_heap.pop_back();

        // pop second min value and put it in right subtree
        std::swap(min_heap.front(), top->right);
        std::pop_heap(min_heap.begin(), min_heap.end(), compare);

        // initialize node with data
        top->character = std::numeric_limits<uint32_t>::max();
        top->frequency = top->left->frequency + top->right->frequency;

        // push node into heap
        min_heap.back() = std::move(top);
        std::push_heap(min_heap.begin(), min_heap.end(), compare);

        // ! notice how we do not pop_back twice and then push_back !
    }
    return std::move(min_heap.front());
}

EncodingTable createEncodingTable(const std::unique_ptr<Node>& root, pal::Metadata metadata)
{
    const auto size = metadata.settings.offset(metadata.productionSize);
    EncodingTable result(size);

    // recursive lambda for epic big dick contest
    std::function<void(const std::unique_ptr<Node>&, uint32_t, uint8_t)> recursion = [&](const auto& root, auto num, auto depth) -> void
    {
        if (root->left  != nullptr) recursion(root->left , num * 2 + 1, depth + 1);
        if (root->right != nullptr) recursion(root->right, num * 2, depth + 1);
        if (root->character != std::numeric_limits<uint32_t>::max()) result[root->character] = std::pair(num, depth);
    };

    recursion(root, 0, 0);
    return result;
}

void printEncodingTable(const EncodingTable& table)
{
    for(size_t i = 0; i < table.size(); i++)
    {
        if(table[i].second == 0) continue;

        // using a bitset to convert numbers to bit strings
        std::bitset<sizeof(uint32_t) * 8> set(table[i].first);
        const auto str = set.to_string();

        std::cout << char(i) << ": " << str.substr(str.size() - table[i].second) << '\n';
    }
}

void dotHuffmanTree(const std::string& path, const std::unique_ptr<Node>& root)
{
    const auto dot = path + ".dot";
    const auto png = path + ".png";

    std::ofstream file(dot);
    file << "digraph G{";

    uint32_t j = 10000000;
    const auto swap = [&](auto i){ if(i == std::numeric_limits<uint32_t>::max()){ return j++; } else return i; };

    std::function<void(const std::unique_ptr<Node>&, uint32_t, uint8_t)> recursion = [&](const auto& root, auto num, auto depth) -> void
    {
        if(root == nullptr) return;
        const auto c = swap(root->character);
        file << num << " -> " << c << '\n';

        if (root->left  != nullptr) recursion(root->left , c, depth + 1);
        if (root->right != nullptr) recursion(root->right, c, depth + 1);
    };

    recursion(root, -1, 0);

    file << "}";


    system(("dot -Tpng " + dot + " -o " + png + "&").c_str());
}

}
