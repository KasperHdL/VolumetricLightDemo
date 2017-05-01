#pragma once

#include "DataPath.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
class FileLoader{
    public:

    static std::string load_file_as_string(string filename){
        std::ifstream file(DataPath::get(filename));

        std::string vert((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
        file.close();

        return vert;
    }

};
