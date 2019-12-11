// =====================================================================
// @name: sequitur.cpp
// @project: SIZE--
// @author: Mano Marichal
// @date: 18.11.19
// @copyright: BA2 Informatica - Mano Marichal - University of Antwerp
// @description: 
// =====================================================================

#include "sequitur.h"
#include <iostream>
#include <fstream>
using namespace algorithm::sequitur;

void Encoder::erase(Node *node)
{
    if (node->previous != nullptr)
    {
        node->previous->next = node->next;
    } else head = node->next;
    if (node->next != nullptr) node->next->previous = node->previous;
}

std::vector<uint32_t> Encoder::NodesToVector(Node *start)
{
    std::vector<uint32_t> variables;
    while (start != nullptr)
    {
        variables.emplace_back(start->value);
        start = start->next;
    }
    return variables;
}

void Encoder::replaceByRule(Node *node, uint32_t rule)
{
    node->value = rule;
    erase(node->previous);
}

void Encoder::linkSymbol(Node* &node)
{
    Digram digram = {node->previous->value, node->value};
    auto it = index.find(digram);

    if (rules.find(digram) != rules.end())
    {
        if (node->previous->previous != nullptr) index.erase(std::make_pair(node->previous->previous->value, digram.first));
        replaceByRule(node, rules.at(digram).first);
        linkSymbol(node);
    }

    else if (it == index.end()) // digram does not exists
    {
        index.try_emplace(std::move(digram), node);
        return;
    }

    else if (index.at(digram) == node->previous or index.at(digram) == node)    // digram overlaps
    {
        return;
    }
    else
    {
        // we make a rule for the current digram
        rules[digram] = std::make_pair(begin, 1);
        productions.emplace_back(Production{digram.first, digram.second});
        Node * location = index.at(digram);

        // we delete all the digrams resulting the replacement of the 2 digrams
        index.erase(digram);
        if (node->previous->previous != nullptr) index.erase(std::make_pair(node->previous->previous->value, digram.first));
        if (location->previous->previous != nullptr) index.erase(std::make_pair(location->previous->previous->value, digram.first));
        index.erase(std::make_pair(digram.second, location->next->value));

        // we replace both digrams by the rule
        replaceByRule(node, begin);
        replaceByRule(location, begin);

        // we check for newly found digrams at the first location
        if (location->previous != nullptr)
        {
            linkSymbol(location);
        }
        if (location->next != nullptr)
        {
            linkSymbol(location->next);
        }

        // we check for the newly found digrams at the current location
        linkSymbol(node);
        begin++;
    }

}

void Encoder::decode(uint32_t c, uint32_t begin)
{
    if (c < begin)
    {
        std::cout << char(c) << std::flush;
        return;
    }
    else
    {
        for (auto rule:rules)
        {
            if (rule.second.first == c)
            {
                decode(rule.first.first, begin);
                decode(rule.first.second, begin);
                return;
            }
        }
    }
}
// TODO:rule utility, thomas adt muur afstemmen
std::tuple<Settings, std::vector<Variable>, std::vector<Production>> Encoder::compress(
        std::vector<unsigned char> string)
{
    Settings settings(Settings::Flags::no_flags);
    productions.reserve(string.size());

    begin = settings.begin();

    nodes.reserve(string.size());
    for (auto var:string)
    {
        nodes.emplace_back(Node(var));
    }

    for (uint i=0;i<nodes.size();i++)
    {
        if (i != 0) nodes[i].previous = &nodes[i-1];
        if (i != nodes.size()-1) nodes[i].next = &nodes[i+1];
    }

    head = &nodes[0];
    Node* node = &nodes[1];     // we start from the second symbol
    while (node != nullptr)
    {
        linkSymbol(node);
        node = node->next;
    }

    std::vector<Variable> variables = NodesToVector(head);

    return std::make_tuple(settings, std::move(variables), std::move(productions));
}

std::tuple<Settings, std::vector<Variable>, std::vector<Production>> algorithm::sequitur::compress(
        std::vector<unsigned char> string)
{
    return Encoder().compress(std::move(string));
}
