#include "hardware.hpp"
#include<iostream>
using namespace std;

char memory[100][4];
CPU cpu;
void CPU :: reset(){
        for(int i=0;i<4;i++){
            R[i]=' ';
            IR[i]= ' ';
        }
        IC=0;
        C= false;
        SI= 0;
    }

void initMemory(){
    for(int i=0;i<100;i++){
        for(int j=0;j<4; j++){
            memory[i][j]='-';
        }
    }
}
char buffer[40];