#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

char memory[300][4];
bool frameUsed[30];
int PTR;
ifstream input;
ofstream output;
bool jobTerminate = false;

struct CPU {
    char R[4], IR[4];
    int IC;
    bool C;
    int SI, PI, TI;
    void reset() {
        IC = 0; C = false;
        SI = PI = TI = 0;
        for(int i=0; i<4; i++) R[i] = IR[i] = ' ';
    }
} cpu;

struct PCB {
    int TTL, TLL, TTC, LLC;
} pcb;

void init() {
    for(int i=0; i<300; i++)
        for(int j=0; j<4; j++) memory[i][j] = '-';
    for(int i=0; i<30; i++) frameUsed[i] = false;
}

int allocateFrame() {
    int f = rand() % 30;
    while(frameUsed[f]) f = rand() % 30;
    frameUsed[f] = true;
    return f;
}

int addressMap(int VA) {
    if(VA < 0 || VA >= 100) return -1;
    int page = VA / 10;
    int offset = VA % 10;
    int pte = PTR + page;
    if(memory[pte][0] == '*') return -1;
    int frame = (memory[pte][2]-'0')*10 + (memory[pte][3]-'0');
    return frame * 10 + offset;
}

void MOS(int RA = -1) {
    if(cpu.TI == 2) {
        output << "Time Limit Exceeded\n\n";
        jobTerminate = true; return;
    }
    if(cpu.PI == 1) {
        output << "Opcode Error\n\n";
        jobTerminate = true; return;
    }
    if(cpu.PI == 2) {
        output << "Operand Error\n\n";
        jobTerminate = true; return;
    }
    if(cpu.PI == 3) {
        string op = ""; op += cpu.IR[0]; op += cpu.IR[1];
        if(op == "GD" || op == "SR") {
            int frame = allocateFrame();
            int VA = (cpu.IR[2]-'0')*10 + (cpu.IR[3]-'0');
            int pte = PTR + (VA/10);
            memory[pte][0]='0'; memory[pte][1]='0';
            memory[pte][2]=(frame/10)+'0'; memory[pte][3]=(frame%10)+'0';
            cpu.PI = 0; cpu.IC--; 
            return;
        } else {
            output << "Invalid Page Fault\n\n";
            jobTerminate = true; return;
        }
    }
    if(cpu.SI == 1) {
        string data;
        if(!getline(input, data) || data.substr(0,4) == "$END") {
            output << "Out of Data\n\n";
            jobTerminate = true; return;
        }
        for(int i=0; i<data.length() && i<40; i++) {
            memory[RA + (i/4)][i%4] = data[i];
        }
    } else if(cpu.SI == 2) {
        pcb.LLC++;
        if(pcb.LLC > pcb.TLL) {
            output << "Line Limit Exceeded\n\n";
            jobTerminate = true; return;
        }
        for(int i=0; i<10; i++) {
            for(int j=0; j<4; j++) if(memory[RA+i][j] != '-') output << memory[RA+i][j];
        }
        output << endl;
    } else if(cpu.SI == 3) {
        output << "Normal Termination\n\n";
        jobTerminate = true;
    }
    cpu.SI = 0;
}

void execute() {
    jobTerminate = false;
    while(!jobTerminate) {
        int RA_inst = addressMap(cpu.IC);
        if(RA_inst == -1) { cpu.PI = 3; MOS(); continue; }

        for(int i=0; i<4; i++) cpu.IR[i] = memory[RA_inst][i];
        cpu.IC++;

        pcb.TTC++;
        if(pcb.TTC > pcb.TTL) cpu.TI = 2;

        if(cpu.IR[0] == 'H') { cpu.SI = 3; MOS(); continue; }
        if(cpu.TI == 2) { MOS(); continue; }

        string op = ""; op += cpu.IR[0]; op += cpu.IR[1];
        if(op != "GD" && op != "PD" && op != "LR" && op != "SR" && op != "CR" && op != "BT") {
            cpu.PI = 1; MOS(); continue;
        }
        if(!isdigit(cpu.IR[2]) || !isdigit(cpu.IR[3])) {
            cpu.PI = 2; MOS(); continue;
        }

        int VA = (cpu.IR[2]-'0')*10 + (cpu.IR[3]-'0');
        int RA_op = addressMap(VA);

        if(op == "GD") {
            if(RA_op == -1) { cpu.PI = 3; MOS(); } else { cpu.SI = 1; MOS(RA_op); }
        } else if(op == "PD") {
            if(RA_op == -1) { cpu.PI = 3; MOS(); } else { cpu.SI = 2; MOS(RA_op); }
        } else if(op == "LR") {
            if(RA_op == -1) { cpu.PI = 3; MOS(); } else { for(int i=0; i<4; i++) cpu.R[i]=memory[RA_op][i]; }
        } else if(op == "SR") {
            if(RA_op == -1) { cpu.PI = 3; MOS(); } else { for(int i=0; i<4; i++) memory[RA_op][i]=cpu.R[i]; }
        } else if(op == "CR") {
            if(RA_op == -1) { cpu.PI = 3; MOS(); } else { cpu.C=true; for(int i=0; i<4; i++) if(cpu.R[i]!=memory[RA_op][i]) cpu.C=false; }
        } else if(op == "BT") {
            if(cpu.C) cpu.IC = VA;
        }
    }
}

void load() {
    string line;
    int loadVA = 0;
    while(getline(input, line)) {
        if(line.empty()) continue;
        if(line.substr(0,4) == "$AMJ") {
            init(); cpu.reset();
            pcb.TTL = stoi(line.substr(8,4));
            pcb.TLL = stoi(line.substr(12,4));
            pcb.TTC = pcb.LLC = 0;
            loadVA = 0;
            PTR = allocateFrame() * 10;
            for(int i=0; i<10; i++) for(int j=0; j<4; j++) memory[PTR+i][j] = '*';
        } else if(line.substr(0,4) == "$DTA") {
            cpu.IC = 0; 
            execute();
        } else if(line.substr(0,4) == "$END") {
            continue;
        } else {
            int frame = allocateFrame();
            int page = loadVA / 10;
            memory[PTR + page][0] = '0'; memory[PTR + page][1] = '0';
            memory[PTR + page][2] = (frame/10) + '0'; memory[PTR + page][3] = (frame%10) + '0';
            for(int i=0; i<line.length() && i<40; i++) {
                memory[frame*10 + (i/4)][i%4] = line[i];
            }
            loadVA += 10;
        }
    }
}

int main() {
    srand(time(0));
    input.open("input.txt");
    output.open("output.txt");
    load();
    input.close();
    output.close();
    return 0;
}