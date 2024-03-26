/*============================================================================================================
FILENAME        :CU.c
DESCRIPTION     :Emulate the function of a control unit using C/ C++ program
AUTHOR          :Zach Riane I. Machacon, Eldridge Aaron S. Miole, Dave Nelson F. Ogue, John Jason C. Zamora
CREATED ON      :18 March 2024
============================================================================================================*/

#include <stdio.h>
#include <unistd.h>
#include <math.h>

unsigned char dataMemory[2048];
unsigned char ioBuffer[32];

int CU();
void initMemory();
void outputPC(unsigned short PC);
void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand, unsigned short MBR, unsigned short IOBR);

int main()
{
    initMemory();
    if (CU() == 1)
    {
        printf("Program ran successfully!");
    }
    else
    {
        printf("Error encountered, program terminated!");
    }
}

int CU(void)
{
    unsigned short operand, IR, PC, MAR, MBR, IO_AR, IO_BR, inst_code;
    unsigned char progMemoryStart = 0x000;
    unsigned short dataMemoryStart = 0x400;

    PC = progMemoryStart;
    printf("Initializing Main Memory...\n");
    while (inst_code != 0x1F)
    {
        // Fetching upper byte
        IR = dataMemory[PC];
        IR = IR << 8;
        PC++;

        // Fetching lower byte
        IR = IR | dataMemory[PC];
        PC++;

        inst_code = IR >> 11;  // Gets upper 5 bits for instruction
        operand = IR & 0x07FF; // Gets lower 11 bits for operand

        outputPC(PC);

        if (inst_code == 0x01)
        { // Write to memory (WM)
            MAR = operand;
            dataMemory[MAR] = MBR;
        }
        else if (inst_code == 0x02)
        { // Read from Memory (RM)
            MAR = operand;
            MBR = dataMemory[MAR];
        }
        else if (inst_code == 0x03)
        { // branch (BR)
            PC = operand;
        }
        else if (inst_code == 0x04)
        { // read from IO buffer (RIO)
            IO_AR = operand;
            IO_BR = ioBuffer[IO_AR];
        }
        else if (inst_code == 0x05)
        { // write to IO buffer (WIO)
            IO_AR = operand;
            ioBuffer[IO_AR] = IO_BR;
        }
        else if (inst_code == 0x06)
        { // write data to MBR (WB)
            MBR = operand & 0xFF;
        }
        else if (inst_code == 0x07)
        { // write data to IOBR (WIB)
            IO_BR = operand & 0xFF;
        }
        else if (inst_code == 0x08)
        { // write data to IOBR (WIB)
            IO_BR = operand & 0xFF;
        }       
        else if(inst_code != 0x1F)
        {
            return 0;
        }
        outputDisplay(IR, inst_code, operand, MBR, IO_BR);
    }
    return 1;
}

void initMemory()
{
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

void outputPC(unsigned short PC) {
    printf("***********************************************\n");
    printf("PC \t \t: 0x%03X\n", PC - 2);
    printf("Fetching instruction...\n");
}

void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand, unsigned short MBR, unsigned short IOBR)
{
    printf("IR \t \t: 0x%X\n", IR);
    printf("Instruction Code: 0x%02X\n", inst_code);
    printf("Operand \t: 0x%03X\n", operand);
    if (inst_code == 0x01)
    {
        printf("Instruction \t: WM\n");
        printf("Writing data to memory...\n");
    }
    else if (inst_code == 0x02)
    {
        printf("Instruction \t: RM\n");
        printf("Reading data from memory...\n");
        printf("MBR \t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == 0x03)
    {
        printf("Instruction\t: BR\n");
        printf("Branching to 0x%X on the next cycle.\n", operand);
    }
    else if (inst_code == 0x07)
    {
        printf("Instruction\t: WIB\n");
        printf("Loading data to IOBR...\n");
        printf("IOBR \t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == 0x05)
    {  
        printf("Instruction\t: WIO\n");
        printf("Writing to IO Buffer...\n");
    }
    else if (inst_code == 0x06)
    {
        printf("Instruction\t: WB\n");
        printf("Loading data to MBR...\n");
        printf("MBR\t\t: 0x%02X\n", MBR); 
    }
    else if (inst_code == 0x1F) 
    {
        printf("Instruction\t: EOP\n");
        printf("End of program.\n\n");
    }
}