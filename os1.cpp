#include<iostream>
#include "os.hpp"
using namespace std;

void OS:: load(){
    string line;
    //int mem=0;
    while(getline(input, line)){
        if(line.empty()) continue;
        if(line.substr(0,4)=="$AMJ"){
            initMemory();
            cpu.reset();
            //mem=0;
        }
        else if(line.substr(0,4)=="$DTA"){
            execute();
        }
        else if(line.substr(0,4)=="$END"){
            continue;
        }
        else{
            int mem=0;
            for(int i=0;i<line.length() && mem<100; i+=4){
                for(int j=0;j<4;j++){
                    if(i+j<line.length())
                        memory[mem][j]= line[i+j];
                    else 
                        memory[mem][j]=' ';
                }
                mem++;
            }
        }
    }
}

void OS:: execute(){
    while(true){
        if (cpu.IC >= 100 ) { 
             break; 
        }
        for(int i=0;i<4;i++){
            cpu.IR[i]=memory[cpu.IC][i];
        }

        //
        if(cpu.IR[0] == 'H') {
            cpu.SI = 3;
            MOS(0);
            return; // EXIT execution loop immediately
        }
        //


        //numerical part of the instruction converted to int 
        int address=(cpu.IR[2]- '0')*10 +(cpu.IR[3]-'0');
        cpu.IC++;

        // GD instruction
        if(cpu.IR[0]=='G' && cpu.IR[1]=='D'){
            cpu.SI=1;
            MOS(address);
        }
        else if(cpu.IR[0]=='P' && cpu.IR[1]=='D'){  //pd
            cpu.SI=2;
            MOS(address);
        }
        
        else if(cpu.IR[0]=='L' && cpu.IR[1]=='R'){   //lr
            for(int i=0;i<4;i++){
                cpu.R[i] = memory[address][i];               
            }
        }
        else if(cpu.IR[0]=='S' && cpu.IR[1]=='R'){
            for(int i=0;i<4;i++){
                memory[address][i]=cpu.R[i];
            }
        }
        else if(cpu.IR[0]=='C' && cpu.IR[1]=='R'){
            bool match=true;
            for(int i=0;i<4;i++){
                if(cpu.R[i]!=memory[address][i]){
                    match=false;
                    break;
                }
            }
            cpu.C=match;
        }
        else if(cpu.IR[0]=='B' && cpu.IR[1]=='T'){
            if(cpu.C==true){
                cpu.IC=address;
            }
        }

    }
}

//master mode, interrupt handling
void OS::MOS(int a){
    if(cpu.SI==1){        //card reader interrupt
        string data;
        
        if(getline(input, data)){
            if(data.substr(0,4) == "$END") return;
            int charIdx=0;
            for(int i=0;i<10;i++){
                for(int j=0;j<4;j++){
                    if(charIdx<data.length())
                        memory[a+i][j]=data[charIdx++];
                    else
                        memory[a+i][j]=' ';
                }
            }
        }
    }
    else if(cpu.SI==2){     //line printer
        for(int i=0;i<10;i++){
            for(int j=0;j<4;j++){
                if(memory[a+1][j]=='-') output<<' ';
                else 
                    output<<memory[a+i][j];
            }
        }
        output<<endl;
        cpu.SI = 0;
    }
    else if(cpu.SI==3){     //halt
        output<<"\n\n";
        output<<"-----------------------------------"<<endl;
    }
    cpu.SI=0;
}