#ifndef OS_HPP
#define OS_HPP
#include <fstream>
#include <string>
#include "hardware.hpp"

class OS {
public:
    std::ifstream input;
    std::ofstream output;

    void load();      
    void execute();   
    void MOS(int i);       
};

#endif