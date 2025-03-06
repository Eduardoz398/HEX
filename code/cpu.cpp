#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <unordered_set>

using namespace std;

#define MEMORY_SIZE 0xffff
#define DATA_MEMORY_SIZE 0xffff
#define STACK_SIZE 0xffff
#define STACK_START 0xAA
#define REG_FULL_SIZE 0xC
#define REG_SIZE 0x07

#define PC 0x08
#define SP 0x09
#define IR 0x0A
#define FLAGS 0x0B

#define DATA_TYPE_MEMORY 0x0000
#define DATA_TYPE_STACK 0x0001
#define DATA_TYPE_DATA_MEMORY 0x0002

#define FLAG_Z 0x01 // Zero
#define FLAG_S 0x80 // Signal
#define FLAG_C 0x02 // Carry
#define FLAG_V 0x04 // Overflow

#define JP 0
#define MOV 1
#define STR 2
#define LDR 3
#define ADD 4
#define SUB 5
#define MUL 6
#define AND 7
#define OR 8
#define NOT 9
#define XOR 10
#define SHR 11
#define SHL 12
#define ROR 13
#define ROL 14

class CPU
{
private:
    uint8_t stack[STACK_SIZE] = {STACK_START};
    uint8_t memory[MEMORY_SIZE] = {0x00};
    uint8_t data_memory[DATA_MEMORY_SIZE] = {0x00};
    uint16_t reg[REG_FULL_SIZE] = {0x00};

    bool running = false;

public:
    std::unordered_set<uint16_t> memoryAddress;
    CPU()
    {
        reg[PC] = 0x0000;
        reg[SP] = 0x8200;
        reg[IR] = 0x0000;
        reg[FLAGS] = 0x0000;
    }

    void set_flag(uint8_t flag, bool value)
    {
        value ? reg[FLAGS] |= (1 << flag) : reg[FLAGS] &= ~(1 << flag);
    }

    bool check_flag(uint8_t flag)
    {
        return (reg[FLAGS] & (1 << flag)) != 0;
    }

    void save_to_memory(uint16_t address, uint16_t value)
    {
        save_to_data(DATA_TYPE_MEMORY, address, value);

        memoryAddress.insert(address);
    }

    void save_to_data(uint8_t type, uint16_t address, uint16_t value)
    {
        uint8_t left = value & 0xFF;
        uint8_t right = (value >> 8) & 0xFF;

        if (type == DATA_TYPE_MEMORY)
        {
            memory[address] = left;
            memory[address + 1] = right;
        }
        else if (type == DATA_TYPE_STACK)
        {
            stack[address] = left;
            stack[address + 1] = right;
        }
        else if (type == DATA_TYPE_DATA_MEMORY)
        {
            data_memory[address] = left;
            data_memory[address + 1] = right;
        }
    }

    uint16_t get_from_data(uint8_t type, uint16_t address)
    {
        if (type == DATA_TYPE_MEMORY)
        {
            return (memory[address] | (memory[address + 1] << 8));
        }
        else if (type == DATA_TYPE_STACK)
        {
            return (stack[address] | (stack[address + 1] << 8));
        }
        else
        {
            return (data_memory[address] | (data_memory[address + 1] << 8));
        }
    }

    int complement_two(int number, int bits)
    {
        return (1 << bits) - number;
    }

    void fetch_instruction()
    {
        reg[IR] = get_from_data(DATA_TYPE_MEMORY, reg[PC]);
        reg[PC] += 2;
    }

    void debug(bool only_flags = false)
    {
        if (only_flags)
        {
            goto flags;
        }

        printf("\n--- CPU STATUS ---\n");

        printf("PC: 0x%04x\n", reg[PC]);
        printf("SP: 0x%04x\n", reg[SP]);

    flags:
        printf("FLAGS: 0x%04x\n", reg[FLAGS]);
        printf("CARRY: %d\n", check_flag(FLAG_C));
        printf("OVERFLOW: %d\n", check_flag(FLAG_V));
        printf("ZERO: %d\n", check_flag(FLAG_Z));
        printf("SIGNAL: %d\n", check_flag(FLAG_S));

        if (only_flags)
            return;

        printf("\n==== General Purpose Registers ====\n");

        for (int i = 0; i <= REG_SIZE; i++)
        {
            printf("Register R%d: 0x%04x\n", i, reg[i]);
        }

        printf("\n==== Memory ====\n");

        for (int i = 0; i < MEMORY_SIZE; i += 2)
        {
            if (i + 1 >= MEMORY_SIZE)
                break;
            uint16_t value = get_from_data(DATA_TYPE_MEMORY, i);

            if (value != 0x00)
            {
                printf("Memory = %04x: 0x%04x\n", i, value);
            }
        }

        printf("\n==== Stack ====\n");

        for (int i = 0; i < STACK_SIZE; i += 2)
        {
            if (i + 1 >= STACK_SIZE)
                break;

            uint16_t value = get_from_data(DATA_TYPE_STACK, i);

            if (value != STACK_START && value != 0x00)
            {
                printf("Stack = %04x: 0x%04x\n", i, value);
            }
        }

        printf("\n==== DATA MEMORY ====\n");

        for (int i = 0; i < DATA_MEMORY_SIZE; i += 2)
        {
            if (i + 1 >= DATA_MEMORY_SIZE)
                break;

            uint16_t value = get_from_data(DATA_TYPE_DATA_MEMORY, i);

            if (value != 0x00)
            {
                printf("DATA MEMORY = %04x: 0x%04x\n", i, value);
            }
        }

        printf("==== END MEMORY DATA ====\n");
    }

    void update_flags(uint32_t result, uint16_t rm, uint16_t rn)
    {
        set_flag(FLAG_C, result > 0xFFFF);

        uint16_t most_bit_rm = (rm & 0x8000) >> 15;
        uint16_t most_bit_rn = (rn & 0x8000) >> 15;
        uint16_t most_bit_result = (result & 0x8000) >> 15;

        set_flag(FLAG_V, most_bit_rm == most_bit_rn && most_bit_result != most_bit_rm);
    }

    void decode_and_execute()
    {

        uint16_t instruction = reg[IR];

        switch (instruction)
        {
        case 0x0000:
            debug();
            return;
        case 0xFFFF:
            printf("HALT\n");
            running = false;
            return;
        }

        uint16_t first_byte = reg[IR] >> 0x0008;
        uint16_t bit = first_byte & 0x0008;
        uint16_t type = bit == 0x0008 ? 0x0001 : 0x0000;

        uint16_t operation = (instruction >> 0x0008) >> 0x0004;
        uint16_t rd = (instruction >> 8) & 0x0007;
        uint16_t rm = (instruction & 0x00E0) >> 0x0005;
        uint16_t rn = (instruction & 0x001C) >> 0x0002;
        uint16_t im_mov = instruction & 0x00FF;
        uint16_t im_jmp = (instruction & 0x07FC) >> 0x0002;
        uint16_t last_two_bits = (instruction & 0x0003);
        uint16_t msb_jmp = (instruction & 0x0400) >> 0x000A;
        uint16_t im_str = (instruction & 0x001F) + ((instruction & 0x0700) >> 0x0003);
        uint16_t im_s = instruction & 0x001F;

        switch (operation)
        {
        case MOV:
            if (type == 0x0000)
            {
                reg[rd] = reg[rm];
                printf("MOV R%d, R%d\n", rd, rm);
            }
            else
            {
                reg[rd] = im_mov;
                printf("MOV R%d, #%04x\n", rd, im_mov);
            }
            break;
        case JP:
            if (type == 0)
            {
                if (last_two_bits == 1)
                {
                    save_to_data(DATA_TYPE_STACK, reg[SP], reg[rn]);
                    reg[SP] -= 2;
                    printf("PSH R%d\n", rn);
                }
                else if (last_two_bits == 2)
                {
                    reg[SP] += 2;
                    reg[rd] = get_from_data(DATA_TYPE_STACK, reg[SP]);
                    printf("POP R%d\n", rd);
                }
                else
                {
                    set_flag(FLAG_Z, reg[rm] == reg[rn]);
                    set_flag(FLAG_S, (int16_t)reg[rm] < (int16_t)reg[rn]);
                    printf("CMP R%d, R%d\n", rm, rn);
                }
            }
            else
            {
                if(im_jmp == 0x1fe)  //Caso em que o jmp pula para si mesmo e o programa deve ser encerrado
                { 
                    running = false;
                }              
                if (last_two_bits == 0)
                {
                    reg[PC] += msb_jmp == 1 ? -complement_two(im_jmp, 9) : im_jmp;
                    printf("JMP #%04x\n", reg[PC]);
                }
                else if (last_two_bits == 1 && check_flag(FLAG_Z) && !check_flag(FLAG_S))
                {
                    reg[PC] += msb_jmp == 1 ? -complement_two(im_jmp, 9) : im_jmp;
                    printf("JEQ #%04x\n", reg[PC]);
                }
                else if (last_two_bits == 2 && !check_flag(FLAG_Z) && check_flag(FLAG_S))
                {
                    reg[PC] += msb_jmp == 1 ? -complement_two(im_jmp, 9) : im_jmp;
                    printf("JLT #%04x\n", im_jmp);
                }
                else if (last_two_bits == 3 && !check_flag(FLAG_Z) && !check_flag(FLAG_S))
                {
                    reg[PC] += msb_jmp == 1 ? -complement_two(im_jmp, 9) : im_jmp;
                    printf("JGT #%04x\n", im_jmp);
                }
            }
            break;

        case LDR:
            reg[rd] = data_memory[reg[rm]];
            printf("LDR R%d, [R%d]\n", rd, rm);
            break;
        case STR:
            if (type == 0)
            {
                data_memory[reg[rm]] = reg[rn];
                printf("STR [R%d], R%d\n", rm, rn);
            }
            else
            {
                data_memory[reg[rm]] = im_str;
                printf("STR [R%d], #%04x\n", rm, im_str);
            }
            break;
        case ADD:
            reg[rd] = reg[rm] + reg[rn];
            update_flags(reg[rm] + reg[rn], reg[rm], reg[rn]);
            printf("ADD R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case SUB:
            reg[rd] = reg[rm] - reg[rn];
            update_flags(reg[rm] - reg[rn], reg[rm], reg[rn]);
            printf("SUB R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case MUL:
            reg[rd] = reg[rm] * reg[rn];
            update_flags(reg[rm] * reg[rn], reg[rm], reg[rn]);
            printf("MUL R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case AND:
            reg[rd] = reg[rm] & reg[rn];
            update_flags(reg[rm] & reg[rn], reg[rm], reg[rn]);
            printf("AND R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case OR:
            reg[rd] = reg[rm] || reg[rn];
            update_flags(reg[rm] || reg[rn], reg[rm], reg[rn]);
            printf("ORR R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case XOR:
            reg[rd] = reg[rm] ^ reg[rn];
            update_flags(reg[rm] ^ reg[rn], reg[rm], reg[rn]);
            printf("XOR R%d, R%d, R%d\n", rd, rm, rn);
            break;
        case NOT:
            reg[rd] = ~reg[rm];
            update_flags(~reg[rm], reg[rd], reg[rm]);
            printf("NOT R%d, R%d\n", rd, rm);
            break;
        case SHR:
            reg[rd] = reg[rm] >> im_s;
            update_flags(reg[rm] >> im_s, reg[rm], im_s);
            printf("SHR R%d, R%d, #%04x\n", rd, rm, im_s);
            break;
        case SHL:
            reg[rd] = reg[rm] << im_s;
            update_flags(reg[rm] << im_s, reg[rm], im_s);
            printf("SHL R%d, R%d, #%04x\n", rd, rm, im_s);
            break;
        case ROR:
            reg[rd] = (reg[rm] >> im_s) | (reg[rm] << (0x0010 - im_s));
            update_flags((reg[rm] >> im_s) | (reg[rm] << (0x0010 - im_s)), reg[rm], im_s);
            printf("ROR R%d, R%d, #%04x\n", rd, rm, im_s);
            break;
        case ROL:
            reg[rd] = (reg[rm] << im_s) | (reg[rm] >> (0x0010 - im_s));
            update_flags((reg[rm] << im_s) | (reg[rm] >> (0x0010 - im_s)), reg[rm], im_s);
            printf("ROL R%d, R%d, #%04x\n", rd, rm, im_s);
            break;
        default:
            printf("Unknown instruction: %04x\n", instruction);
            running = false;
            break;
        }

        if (memoryAddress.find(reg[PC]) == memoryAddress.end())
        {
            running = false;
            return;
        }
    }

    void execute()
    {
        running = true;

        while (running)
        {
            fetch_instruction();
            decode_and_execute();
        }
    }

    void run()
    {
        execute();
        debug();
    }
};
