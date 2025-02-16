#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <bitset>
#include <algorithm>

using namespace std;

class Decode {
public:
    string to_hex(uint16_t value) {
        stringstream ss;
        ss << std::hex << std::uppercase << value;
        return ss.str();
    }

    
};