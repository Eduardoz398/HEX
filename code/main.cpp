#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include "cpu.cpp"
#include "decode/decode.cpp"
#include "utils/color.hpp"

std::string trim(const std::string str)
{
    size_t start = str.find_first_not_of(" \n\r\t");
    size_t end = str.find_last_not_of(" \n\r\t");

    return start == std::string::npos ? "" : str.substr(start, end - start + 1);
}

void load_program(const std::string &filePath, CPU &cpu)
{
    std::ifstream file(filePath);

    if (!file)
    {
        std::cerr << "Error: cannout open " << filePath << std::endl;
        return;
    }

    std::string line, address, value;

    while (std::getline(file, line))
    {
        size_t pos = line.find(':');

        if (pos != std::string::npos)
        {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            uint16_t address = std::stoi(key, 0, 16);
            uint16_t val = std::stoi(value, 0, 16);

            cpu.save_to_memory(address, val);
        }
    }
}

int main()
{
    CPU cpu;
    Decode decode;

    load_program("instruction-06.txt", cpu);

    // cout << color::yellow << decode.to_hex(1);
    cout << color::green;
    cout << "Executing..." << endl;
    cpu.run();
    return 0;
}