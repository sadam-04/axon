#include <iostream>
#include <fstream>
#include <sstream>

std::string loadFile(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file " + filename + "\n";
        exit(1);
    }

    std::ostringstream sstr;
    sstr << file.rdbuf();

    return sstr.str();
}