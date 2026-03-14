#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// =============================================================================
// GLOBAL HARDWARE DECLARATIONS
// =============================================================================

char memory[100][4];
char buffer[40];

class CPU {
public:
    char R[4];   // General Purpose Register
    char IR[4];  // Instruction Register
    int IC;      // Instruction Counter
    bool C;      // Toggle Trigger (Compare result)
    int SI;      // Service Interrupt

    void reset() {
        for (int i = 0; i < 4; i++) {
            R[i] = ' ';
            IR[i] = ' ';
        }
        IC = 0;
        C = false;
        SI = 0;
    }
};

CPU cpu;

void initMemory() {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            memory[i][j] = '-';
        }
    }
}

class OS {
public:
    ifstream input;
    ofstream output;

    void load();      
    void execute();   
    void MOS(int a);       
};

void OS::MOS(int a) {
    if (cpu.SI == 1) {        // GD
        string data;
        if (getline(input, data)) {
            if (data.substr(0, 4) == "$END") return;
            int charIdx = 0;
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 4; j++) {
                    if (charIdx < data.length())
                        memory[a + i][j] = data[charIdx++];
                    else
                        memory[a + i][j] = ' ';
                }
            }
        }
    }
    else if (cpu.SI == 2) {     // PD
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 4; j++) {
                if (memory[a + i][j] == '-') 
                    output << ' ';
                else 
                    output << memory[a + i][j];
            }
        }
        output << endl;
    }
    else if (cpu.SI == 3) {     
        output << "\n-----------------------------------\n" << endl;
    }
    cpu.SI = 0; 
}


void OS::execute() {
    while (true) {
        if (cpu.IC >= 100) break;

        //fetch Instruction
        for (int i = 0; i < 4; i++) {
            cpu.IR[i] = memory[cpu.IC][i];
        }

        if (cpu.IR[0] == 'H') {
            cpu.SI = 3;
            MOS(0);
            return; 
        }

        //address
        int address = (cpu.IR[2] - '0') * 10 + (cpu.IR[3] - '0');
        cpu.IC++;

        // Execute 
        if (cpu.IR[0] == 'G' && cpu.IR[1] == 'D') {
            cpu.SI = 1;
            MOS(address);
        }
        else if (cpu.IR[0] == 'P' && cpu.IR[1] == 'D') {
            cpu.SI = 2;
            MOS(address);
        }
        else if (cpu.IR[0] == 'L' && cpu.IR[1] == 'R') {
            for (int i = 0; i < 4; i++) cpu.R[i] = memory[address][i];
        }
        else if (cpu.IR[0] == 'S' && cpu.IR[1] == 'R') {
            for (int i = 0; i < 4; i++) memory[address][i] = cpu.R[i];
        }
        else if (cpu.IR[0] == 'C' && cpu.IR[1] == 'R') {
            cpu.C = true;
            for (int i = 0; i < 4; i++) {
                if (cpu.R[i] != memory[address][i]) {
                    cpu.C = false;
                    break;
                }
            }
        }
        else if (cpu.IR[0] == 'B' && cpu.IR[1] == 'T') {
            if (cpu.C) cpu.IC = address;
        }
    }
}


void OS::load() {
    string line;
    int mem = 0; // Move this OUTSIDE the while loop
    while (getline(input, line)) {
        if (line.empty()) continue;

        if (line.substr(0, 4) == "$AMJ") {
            initMemory();
            cpu.reset();
            mem = 0; 
            if(line.length() >= 8) {
            string jobId = line.substr(4, 4);
            // output << "JOB ID           : " << jobId << endl;
    }
        }
        else if (line.substr(0, 4) == "$DTA") {
            execute();
        }
        else if (line.substr(0, 4) == "$END") {
            mem = 0;
            continue;
        }
        else {
            for (int i = 0; i < line.length() && mem < 100; i += 4) {
                for (int j = 0; j < 4; j++) {
                    if (i + j < line.length())
                        memory[mem][j] = line[i + j];
                    else
                        memory[mem][j] = ' '; 
                }
                mem++; 
            }
        }
    }
}

// =============================================================================
// MAIN DRIVER
// =============================================================================

int main() {
    OS myOS;
    myOS.input.open("input.txt");
    myOS.output.open("output.txt");

    if (!myOS.input.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    myOS.load();

    myOS.input.close();
    myOS.output.close();

    return 0;
}