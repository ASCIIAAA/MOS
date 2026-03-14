#ifndef HARDWARE_HPP
#define HARDWARE_HPP

extern char memory[100][4]; 
extern char buffer[40];     

class CPU {
public:
    char R[4];   
    char IR[4];  
    int IC;      
    bool C;      
    int SI;      

    void reset();
};

extern CPU cpu;

void initMemory();

#endif