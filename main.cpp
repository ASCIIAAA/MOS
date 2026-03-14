#include "os.hpp"
#include "hardware.hpp"
#include<iostream>
using namespace std;

int main() {
    OS myOS;
    myOS.input.open("input.txt");
    myOS.output.open("output.txt");

    if (!myOS.input.is_open()) {
        cerr <<"Error: Could not open input.txt"<<endl;
        return 1;
    }

    myOS.load(); 

    myOS.input.close();
    myOS.output.close();
    return 0;
}