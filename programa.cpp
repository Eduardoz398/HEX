#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#define PC 8
#define SP 9
#define IR 10

int operation = 0, Rd = 0, Rm = 0, Rn = 0, Im_MOV = 0, Im_JMP = 0, Last_Two_Bits = 0;

/**
 * 
 * compiler program 
 * g++ -o programa programa.cpp -std=c++17 && ./programa
 */
   
void fetch(unsigned int memory[], unsigned int reg[]) {
    reg[IR] = memory[reg[PC]];
    reg[PC] += 2;        
} 

void decode(unsigned int reg[]) { 
    operation = (reg[IR] >> 8) >> 4;
    Rd = (reg[IR] >> 8) & 0x0007;
    Rm = (reg[IR] & 0x00e0) >> 5;   
    Rn = (reg[IR] & 0x001c) >> 2;
    Im_MOV = reg[IR] & 0x00ff;
    Im_JMP = (reg[IR] & 0x07fc) >> 2;
    Last_Two_Bits = (reg[IR] & 0x0003);
}

int type(unsigned int reg[]) { 
    unsigned int first_byte = reg[IR] >> 8;
    unsigned int bit = first_byte & 0x0008;  
    return bit == 0x0008 ? 1 : 0;
}

std::string trim(const std::string str) {
    size_t start = str.find_first_not_of(" \n\r\t");
    size_t end = str.find_last_not_of(" \n\r\t");

    return start == std::string::npos ? "" : str.substr(start, end - start + 1);
}

int main() {
    int Z = 0, C = 0;
    unsigned int reg[11];

    for (int i = 0; i <= 7; i++) {
       reg[i] = 0x0000;
    }

    reg[PC] = 0x0000;
    reg[SP] = 0x8200;
    reg[IR] = 0x0000;

    unsigned int memory[0xffff];          
    unsigned int stack[0xffff];

    std::ifstream file("ins.txt");
    std::string line; 
    
    for (int i = 0; i <= 0xffff; i += 2) { // i marca o endereço.
        memory[i] = 0x0000;
        stack[i] = 0xaaaa;
    }

    while (std::getline(file, line)) {
        size_t pos = line.find(':');

        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            memory[std::stoi(key, 0, 16)] = std::stoi(value, 0, 16);
        }
    }

    int dudu = 0;
    
    while (dudu++ <= 9) {
        fetch(memory, reg);
        decode(reg);

        if (operation == 1) {
            if (type(reg) == 0) {
                reg[Rd] = reg[Rm];
                printf("MOV R%d, R%d\n", Rd, Rm);
            } else {
                reg[Rd] = Im_MOV;
                printf("MOV R%d, #%d\n", Rd, Im_MOV);
            }     
        } else if (operation == 0 && type(reg) == 0) { //Instrução de pilha ou de compação
            if (Last_Two_Bits == 1) {  
                stack[reg[SP]] = reg[Rn];
                reg[SP] -=2;
                printf("PSH R%d\n", Rn);  
            } else if (Last_Two_Bits == 2) {
                reg[SP] +=2;
                reg[Rd] = stack[reg[SP]];    
                printf("POP R%d\n", Rd);
            } else {
                Z = (reg[Rm] == reg[Rn]) ? 1 : 0;  
                C = (reg[Rm] < reg[Rn])? 1:0;
                printf("CMP R%d, R%d\n", Rm, Rn);
            }
        } else if (operation == 4) { //Operações aritméticas, ADD
            reg[Rd] = reg[Rm] + reg[Rn];
            printf("ADD R%d, R%d, R%d\n", Rd, Rm, Rn);
        } else if (operation == 5) { //SUB
            reg[Rd] = reg[Rm] - reg[Rn];
            printf("SUB R%d, R%d, R%d\n", Rd, Rm, Rn);
        } else if (operation == 6) { //MUL
            reg[Rd] = reg[Rm] * reg[Rn];
            printf("MUL R%d, R%d, R%d", Rd, Rm, Rn);
        } else if (operation == 7) { //AND
            reg[Rd] = reg[Rm] & reg[Rn];  
            printf("AND R%d, R%d, R%d", Rd, Rm, Rn);
        } else if (operation == 8) { //OR
            reg[Rd] = reg[Rm] || reg[Rn];
             printf("ORR R%d, R%d, R%d", Rd, Rm, Rn);
        } else if (operation == 9) { //XOR
            reg[Rd] = reg[Rm] ^ reg[Rn];
            printf("XOR R%d, R%d, R%d", Rd, Rm, Rn);
        } else if (operation == 0 && type(reg) == 1) { // Instruções de devsio
            if (Last_Two_Bits == 0) {
                reg[PC] += Im_JMP;
                printf("JMP #%d", Im_JMP);
            }    
        } else if (Last_Two_Bits == 1 && Z == 1 && C == 0) {
            reg[PC] +=Im_JMP;
            printf("JEQ #%d", Im_JMP);
        } else if (Last_Two_Bits == 2 && Z == 0 && C == 1) {
            reg[PC] +=Im_JMP;
            printf("JLT #%d", Im_JMP);
        } else if (Last_Two_Bits == 3 && Z == 0 && C == 0) {
            reg[PC] +=Im_JMP;
            printf("JGT #%d", Im_JMP);
        }    
    }   
}
