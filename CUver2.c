/*============================================================================================================
FILENAME        :CUver2.c
DESCRIPTION     :Modify the Control Unit (CU) on the previous exercise to include the data bus (instruction
                and data), address bus and control bus (lines). Refer to the TRACS document for the
                architecture and bus connections.
AUTHOR          :Zach Riane I. Machacon, Eldridge Aaron S. Miole, Dave Nelson F. Ogue, John Jason C. Zamora
CREATED ON      :18 March 2024
============================================================================================================*/

#include <stdio.h>

unsigned char dataMemory[2048];
unsigned char ioBuffer[32];
unsigned short ADDR;
unsigned char BUS, CONTROL;
int IOM = 0, RW = 0, OE = 0;

void initMemory();
void MainMemory();
void IOMemory();
void outputPC(unsigned short PC);
void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand, unsigned short MBR,
                   unsigned char BUS, unsigned short ADDR, unsigned short MAR, unsigned short IO_AR, unsigned char IO, unsigned short IO_BR, unsigned char FETCH, unsigned char MEMORY,
                   unsigned char CONTROL, int IOM, int RW, int OE);
int CU();

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

    return 0;
}

int CU(void)
{
    unsigned short operand, IR, PC, MAR = 0, MBR, IO_AR, IO_BR, inst_code;
    unsigned char progMemoryStart = 0x000;
    unsigned short dataMemoryStart = 0x400;
    unsigned char FETCH, IO, MEMORY;

    PC = progMemoryStart;
    printf("Initializing Main Memory...\n");
    MainMemory();
    while (inst_code != 0x1F)
    {
        CONTROL = inst_code;
        IOM = 1;
        RW = 0;
        OE = 1;

        // Fetching Instruction (2 Cycles)
        FETCH = 1;
        IO = 0;
        MEMORY = 0;

        // Fetching upper byte
        ADDR = PC;
        MainMemory();

        if (FETCH == 1)
        {
            IR = (int)BUS;
            IR = IR << 8;
            PC++;
            ADDR = PC;
        }

        MainMemory();
        // Fetching lower byte
        if (FETCH == 1)
        {
            IR = IR | BUS;
            PC++;
        }

        inst_code = IR >> 11;  // Gets upper 5 bits for instruction
        operand = IR & 0x07FF; // Gets lower 11 bits for operand

        outputPC(PC);

        if (inst_code == 0x01)
        {                  // Write to memory (WM)
            MAR = operand; // Sets operand to MAR
            FETCH = 0;     // Fetch set to 0 since execution loop
            MEMORY = 1;    // MEMORY is 1 since it deals with main memory
            IO = 0;        // IO set to 0 since it does not deal with IO

            CONTROL = inst_code;
            IOM = 1; // IOM at 1 means that we're accessing main memory
            RW = 1;  // RW at 1 means it is write operation
            OE = 1;  // OE at 1 means that data movement is allowed

            ADDR = MAR; // set ADDR to MAR
            if (MEMORY)
            {              // If MEMORY is being accessed,
                BUS = MBR; // Set BUS to MBR
            }
            MainMemory(); // Run MainMemory
        }
        else if (inst_code == 0x02)
        { // Read from Memory (RM)
            MAR = operand;
            FETCH = 0;
            MEMORY = 1;
            IO = 0;

            CONTROL = inst_code;
            IOM = 1;
            RW = 0;
            OE = 1;

            ADDR = MAR;   // sets ADDR to MAR
            MainMemory(); // However, MainMemory is ran first in order to move from memory -> bus
            if (MEMORY)
            {
                MBR = BUS;
            }
        }
        else if (inst_code == 0x03)
        { // branch (BR)
            CONTROL = inst_code;
            PC = operand;
            OE = 0; // tells the main memory to be locked as it does not require read/write
        }
        else if (inst_code == 0x04)
        { // read from IO buffer (RIO)
            IO_AR = operand;
            FETCH = 0;
            MEMORY = 0;
            IO = 1;

            CONTROL = inst_code;
            IOM = 0; // IOM set to 0 since we're accessing IO Memory
            RW = 0;
            OE = 1;

            ADDR = IO_AR;
            IOMemory();
            if (IO)
            {
                IO_BR = BUS;
            }
        }
        else if (inst_code == 0x05)
        { // write to IO buffer (WIO)
            IO_AR = operand;
            ioBuffer[IO_AR] = IO_BR;

            IO_AR = operand;
            FETCH = 0;
            MEMORY = 0;
            IO = 1;

            CONTROL = inst_code;
            IOM = 0;
            RW = 1;
            OE = 1;

            ADDR = IO_AR;
            if (IO)
            {
                BUS = IO_BR;
            }
            IOMemory();
        }
        else if (inst_code == 0x06)
        { // write data to MBR (WB)
            CONTROL = inst_code;
            MBR = operand & 0xFF;
            OE = 0;
        }
        else if (inst_code == 0x07)
        { // write data to IOBR (WIB)
            CONTROL = inst_code;
            IO_BR = operand & 0xFF;
            OE = 0;
        }
        else if (inst_code == 0x08)
        { // write data to IOBR (WIB)
            CONTROL = inst_code;
            IO_BR = operand & 0xFF;
            OE = 0;
        }
        else if (inst_code != 0x1F)
        {
            return 0;
        }
        outputDisplay(IR, inst_code, operand, MBR,
                      BUS, ADDR, MAR, IO_AR, IO, IO_BR, FETCH, MEMORY,
                      CONTROL, IOM, RW, OE);
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

void outputPC(unsigned short PC)
{
    printf("***********************************************\n");
    printf("PC \t \t: 0x%03X\n", PC - 2);
    printf("Fetching instruction...\n");
}

void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand, unsigned short MBR,
                   unsigned char BUS, unsigned short ADDR, unsigned short MAR, unsigned short IO_AR, unsigned char IO, unsigned short IO_BR, unsigned char FETCH, unsigned char MEMORY,
                   unsigned char CONTROL, int IOM, int RW, int OE)
{

    printf("Address BUS\t: 0x%03X\n", ADDR);
    printf("DATA BUS\t: 0x%02X\n\n", BUS);
    printf("MAR \t\t: 0x%X\n", MAR);
    printf("IO_AR \t\t: 0x%02X\n", IO_AR);
    printf("IO_BR \t\t: 0x%02X\n\n", IO_BR);

    printf("Local Control Signals\n");
    printf("FETCH \t\t: %i\n", FETCH);
    printf("IO \t\t: %i\n", IO);
    printf("MEMORY \t\t: %i\n\n", MEMORY);

    printf("External Control Signals\n");
    printf("Control Signals : 0x%02X\n", CONTROL);
    printf("IO/M \t\t: %i\n", IOM);
    printf("R/W \t\t: %i\n", RW);
    printf("OE  \t\t: %i\n\n", OE);

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
        printf("IOBR \t\t: 0x%02X\n", IO_BR);
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

void MainMemory()
{
    if (IOM == 1)
    {
        if (RW == 0 && OE == 1)
        {
            BUS = dataMemory[ADDR];
        }
        else if (RW == 1 && OE == 1)
        {
            dataMemory[ADDR] = BUS;
        }
    }
}

void IOMemory()
{
    if (IOM == 0)
    {
        if (RW == 0 && OE == 1)
        {
            BUS = ioBuffer[ADDR];
        }
        else if (RW == 1 && OE == 1)
        {
            ioBuffer[ADDR] = BUS;
        }
    }
}