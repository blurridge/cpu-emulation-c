/*============================================================================================================
FILENAME        :CU.c
DESCRIPTION     :Emulate the function of a control unit using C/ C++ program
AUTHOR          :Zach Riane I. Machacon, Eldridge Aaron S. Miole, Dave Nelson F. Ogue, John Jason C. Zamora
CREATED ON      :18 March 2024
============================================================================================================*/

#include <stdio.h>
#include <unistd.h>

unsigned char dataMemory[2048];
unsigned char ioBuffer[32]; 

int CU();
void initMemory();

void main() {
    initMemory();
    if(CU() == 1) {
        printf("Program ran successfully!");
    }
    else {
        printf("Error encountered, program terminated!");
    }
}

int CU(void) {
    unsigned char PC, IR, MAR, MBR, IO_AR, IO_BR, inst_code;
    unsigned short operand;
    unsigned char progMemoryStart = 0x000;
    unsigned short dataMemoryStart = 0x400;

    PC = progMemoryStart;
    while(inst_code != 0x1F) {
        // Fetching upper byte
        IR = dataMemory[PC];
        IR = IR << 8;
        PC++;

        // Fetching lower byte
        IR = IR | dataMemory[PC];
        PC++;

        inst_code = IR >> 11; // Gets upper 5 bits for instruction
        operand = IR & 0x07FF; // Gets lower 11 bits for operand

        if(inst_code == 0x01) {
            MAR = operand;
            dataMemory[MAR] = MBR;
        }
        else if(inst_code == 0x05) {
            IO_AR = operand;
            ioBuffer[IO_AR] = IO_BR;
        }
    }
    return 1;
}

void initMemory() {
    dataMemory[0x000] = 0x30;
    dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C;
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14;
    dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19;
    dataMemory[0x007] = 0x2A;

    dataMemory[0x12A] = 0x38;
    dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28;
    dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8;
    dataMemory[0x12F] = 0x00;
}