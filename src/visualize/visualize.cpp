//============================================================================
// @name        : visualize.cpp
// @author      : Thomas Dooms
// @date        : 12/29/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================

#include "huffman.h"

namespace visualize
{

void huffmanTree(const std::filesystem::path& directory, const std::string& name, const std::unique_ptr<huffman::Node>& root)
{
    std::filesystem::create_directory(directory);
    const auto dot = directory / (name + ".dot");
    const auto png = directory / (name + ".png");

    std::ofstream file(dot);
    file << "digraph G{\n";

    uint32_t curr = 10000000;

    std::function<void(const std::unique_ptr<huffman::Node>&, uint32_t, uint32_t)> recursion = [&](const auto& root, const auto num, const auto depth) -> void
    {
        if(root == nullptr) return;
        if(depth > 8) return;

        uint32_t next;

        if(num == std::numeric_limits<uint32_t>::max()) file << num << " [style=invis];\n";

        if(root->character == std::numeric_limits<uint32_t>::max())
        {
            next = curr;
            file << curr << " [label=\"\"];\n";
            file << num << " -> " << curr++ << '\n';
        }
        else
        {
            next = root->character;
            file << num << " -> " << root->character << '\n';
        }

        if (root->left  != nullptr) recursion(root->left , next, depth + 1);
        if (root->right != nullptr) recursion(root->right, next, depth + 1);
    };

    recursion(root, std::numeric_limits<uint32_t>::max(), 0);

    file << '}';
    file << std::flush;

    system(("dot -Tpng " + dot.string() + " -o " + png.string()).c_str());
    std::filesystem::remove(dot);
}

void parseTree(const std::filesystem::path& directory, const std::string& name, const std::vector<Variable>& string, const std::vector<Production>& productions, pal::Metadata metadata)
{
    std::filesystem::create_directory(directory);
    const auto dot = directory / (name + ".dot");
    const auto png = directory / (name + ".png");

    std::ofstream file(dot);
    const auto& settings = metadata.settings;

    const auto size = std::min(16ul, string.size());

    file << "digraph G{\n";
    file << "splines=false;\n";
    file << "rankdir=BT;\n";
    file << "node [shape = record,height=.1];\n";

    std::array<std::vector<Variable>, 2> buffers;
    buffers[0] = std::vector<Variable>(string.begin(), string.begin() + size);
    buffers[1] = std::vector<Variable>();

    constexpr size_t max_levels = 6;
    for(size_t level = 0; level <= max_levels; level++)
    {
        file << "level" << level << " [fixedsize=true, width=25, height=0.5, label = \"";

        const auto curr = level % 2;
        for(size_t i = 0; i < buffers[curr].size(); i++)
        {
            if(std::isprint(buffers[curr][i]) and buffers[curr][i] < 128)
            {
                file << "<f" << i << ">" << static_cast<char>(buffers[curr][i]);
            }
            else
            {
                file << "<f" << i << ">" << buffers[curr][i];
            }
            if(i != buffers[curr].size() - 1) file << "|";
        }
        file << "\"];\n";

        if(level == max_levels) break;

        // swap buffers and write file
        buffers[not curr].clear();
        if(std::all_of(buffers[curr].begin(), buffers[curr].end(), &Settings::is_byte)) break;

        for(size_t i = 0; i < buffers[curr].size(); i++)
        {
            file << "level" << (level + 1) << " -> " << "level" << level << "[style = invis, weight= 100];\n";
            const auto write_file = [&]()
            {
                file << "level" << level+1 << ":f" << buffers[not curr].size() << " -> " << "level" << level << ":f" << i << ";\n";
            };

            if(Settings::is_byte(buffers[curr][i]))
            {
                write_file();
                buffers[not curr].emplace_back(buffers[curr][i]);
            }
            else if(settings.is_reserved_variable(buffers[curr][i]))
            {
                const auto production = Settings::convert_from_reserved(buffers[curr][i]);

                write_file();
                buffers[not curr].emplace_back(production[0]);
                write_file();
                buffers[not curr].emplace_back(production[1]);
            }
            else
            {
                const auto production = productions[buffers[curr][i] - settings.begin()];

                write_file();
                buffers[not curr].emplace_back(production[0]);
                write_file();
                buffers[not curr].emplace_back(production[1]);
            }
        }
    }

    file << '}';
    file << std::flush;

    system(("dot -Tpng " + dot.string() + " -o " + png.string()).c_str());
    std::filesystem::remove(dot);
}

}