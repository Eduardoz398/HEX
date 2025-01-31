#include <stdio.h>
#include <stdlib.h>
#define PC 8
#define SP 9
#define IR 10

int operation = 0, Rd = 0, Rm = 0, Rn = 0, Im_MOV = 0, Im_JMP = 0, Last_Two_Bits = 0;

   
void fetch(unsigned int memory[], unsigned int reg[]) {
        reg[IR] = memory[reg[PC]];
        reg[PC] +=2;        
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
    if (bit == 0x0008)
        return 1;
    else 
        return 0;                
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
    for (int i = 0; i <= 0xffff; i += 2) { // i marca o endereço.
        memory[i] = 0x0000;
    }
    unsigned int stack[0xffff];
    for(int i = 0; i <= 0xffff; i +=2) {
        stack[i] = 0xaaaa;
    }
    FILE* file;
    file = fopen("hexa.txt", "r");
    for(int i = 0; i <= 20; i +=2) {
       fscanf(file,"%4x", &memory[i]);
      
    }
    while(1) {
        fetch(memory, reg);
        decode(reg);

        if(operation == 0 && type(reg) == 0 && Last_Two_Bits == 0) {
            printf("\n\n");
            for(int i = 0; i <=7;i++) {
                printf("Valor de R%d:  %d\n", i, reg[i]);
            }    
                printf("PC: %x\n SP: %x\n", reg[PC], reg[SP]);
                printf("Z: %d C: %d \n", Z, C);  
            int count = 0;
            for (int i = 0; i < 0x64; i +=2) {
                printf("%04X ", memory[i]);  // Printando em hexadecimal
                count++;
                if (count == 8) {  // A cada 8 valores, pula para a próxima linha
                    printf("\n");
                count = 0;
                }
            }
            break; 
        }    
        else if (operation == 1) {
            if(type(reg) == 0) {
                reg[Rd] = reg[Rm];
                printf("MOV R%d, R%d\n", Rd, Rm);
            }
            else {
                reg[Rd] = Im_MOV;
                printf("MOV R%d, #%d\n", Rd, Im_MOV);
            }     
        }  
     //Instrução de pilha ou de compação
     else if(operation == 0 && type(reg) == 0) {
        if(Last_Two_Bits == 1) {  
            stack[reg[SP]] = reg[Rn];
            reg[SP] -=2;
            printf("PSH R%d\n", Rn);  
        }    
        else if(Last_Two_Bits == 2) {
            reg[SP] +=2;
            reg[Rd] = stack[reg[SP]];    
            printf("POP R%d\n", Rd);
        }    
        else {
            Z = (reg[Rm] == reg[Rn]) ? 1 : 0;  
            C = (reg[Rm] < reg[Rn])? 1:0;
            printf("CMP R%d, R%d\n", Rm, Rn);
        }
    } 
    //Operações aritméticas
        //ADD    
        else if(operation == 4) {
            reg[Rd] = reg[Rm] + reg[Rn];
            printf("ADD R%d, R%d, R%d\n", Rd, Rm, Rn);
        }    
        //SUB
        else if(operation == 5) {
            reg[Rd] = reg[Rm] - reg[Rn];
            printf("SUB R%d, R%d, R%d\n", Rd, Rm, Rn);
        }    

        //MUL
        else if(operation == 6) {
            reg[Rd] = reg[Rm] * reg[Rn];
            printf("MUL R%d, R%d, R%d", Rd, Rm, Rn);
        }    
        //AND
        else if(operation == 7) {
            reg[Rd] = reg[Rm] & reg[Rn];  
            printf("AND R%d, R%d, R%d", Rd, Rm, Rn);
        } 
        //OR
        else if(operation == 8)  {
            reg[Rd] = reg[Rm] || reg[Rn];
             printf("ORR R%d, R%d, R%d", Rd, Rm, Rn);
        }
        //XOR
         else if(operation == 9)  {
            reg[Rd] = reg[Rm] ^ reg[Rn];
            printf("XOR R%d, R%d, R%d", Rd, Rm, Rn);
        }
    // Instruções de devsio
        else if(operation == 0 && type(reg) == 1) {
            if(Last_Two_Bits == 0) {
                reg[PC] += Im_JMP;
                printf("JMP #%d", Im_JMP);
            }    
        }
            else if(Last_Two_Bits == 1 && Z == 1 && C == 0) {
                reg[PC] +=Im_JMP;
                printf("JEQ #%d", Im_JMP);
            }    
            else if(Last_Two_Bits == 2 && Z == 0 && C == 1) {
                reg[PC] +=Im_JMP;
                printf("JLT #%d", Im_JMP);
            }    
            else if(Last_Two_Bits == 3 && Z == 0 && C == 0) {
                reg[PC] +=Im_JMP;
                printf("JGT #%d", Im_JMP);
            }    
    }   
}
