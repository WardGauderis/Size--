//============================================================================
// @name        : palDecoder.cpp
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     :
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description :
//============================================================================

#include <experimental/filesystem>
#include <fstream>
#include <chrono>
namespace compare
{
    std::string getAlgoName(int i)
    {
        switch(i)
        {
            case 1: return "None";
            case 2: return "Bisection";
            case 3: return "Bisection++";
            case 4: return "LCA";
            case 5: return "OLCA";
            case 6: return "Sequitur";
            case 7: return "LZW";
            default: return "something went wrong";
        }
    };

    void compareAlgorithms(std::string filename)
    {
        int final{};
        for (int i=filename.size();i>=0;i--)
        {
            if (filename[i] == '/') break;
            final = i;
        }

        uint32_t minsize = std::numeric_limits<uint32_t>::max();
        uint32_t mintime = std::numeric_limits<uint32_t>::max();
        uint32_t mintimealgo{};
        uint32_t minsizealgo{};

        std::string name = filename.substr(final, filename.size()-final);

        // create table
        std::fstream file;
        file.open("./comparison_" + name + ".html", std::fstream::out);

        file << "<style>\n"
                "table {\n"
                "   border-collapse: collapse;\n"
                "}\n"
                "td, th {\n"
                "   border: 1px solid orange;\n"
                "   padding: 15px;\n"
                "}\n"
                "</style>\n";

        file << "Comparison for file: " << name;
        file << "<br>Original size: " << std::experimental::filesystem::file_size(filename) << "B";
        file << "<table>";
        for (int i=0;i<=6;i++) {
            file << "<tr>";

            std::string command = "./SIZE-- -c" + std::to_string(i) + " " + filename;
            std::chrono::time_point start = std::chrono::high_resolution_clock::now();
            if (i != 0) system(command.c_str());
            std::chrono::time_point end = std::chrono::high_resolution_clock::now();
            uint32_t dur = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();


            if (dur<mintime and i != 0)
            {
                mintimealgo = i;
                mintime = dur;
            }
            uint32_t size = std::numeric_limits<uint32_t>::max();
            if (i !=0) size = std::experimental::filesystem::file_size( "./" + name + ".pal");

            if (size<minsize and i !=0)
            {
                minsizealgo = i;
                minsize = size;
            }

            for (int j=0;j<=3;j++) {
                file << "<th>";
                if (i==0)
                {
                    switch(j)
                    {
                        case 0: file << "algorithm"; break;
                        case 1: file << "size"; break;
                        case 2: file << "time"; break;
                        case 3: file << "compression\nratio"; break;
                    }
                    continue;
                }
                switch(j)
                {
                    case 0: file << getAlgoName(i); break;
                    case 1: file << size << "B"; break;
                    case 2: file << dur << "ms"; break;
                    case 3: file << float(size)/float(std::experimental::filesystem::file_size(filename))*100 << "%";
                }
                file << "</th>\n";
            }
            file << "</tr>";
        }
        file << "</table>";
        file << "The algorithm achieving the highest compression ration is: " + getAlgoName(minsizealgo)
        << "<br>The fastest algorithm is: " + getAlgoName(mintimealgo);
        file.close();

        std::string command = "./SIZE-- -c" + std::to_string(minsizealgo) + " " + filename;
        system(command.c_str());
    }
}