/*============================================================================================================
FILENAME        :ALU-CU.c
DESCRIPTION     :Revise the Arithmetic and Logical Unit (ALU) to conform to the computer architecture of TRACS.
                Integrate the Control Unit (CU) where the CU sets the data on the bus then the ALU performs
                the arithmetic/logic operations.
AUTHOR          :Zach Riane I. Machacon, Eldridge Aaron S. Miole, Dave Nelson F. Ogue, John Jason C. Zamora
CREATED ON      :09 April 2024
============================================================================================================*/

#include <stdio.h>

unsigned char dataMemory[2048];
unsigned char ioBuffer[32];
unsigned char FLAGS; // 7 - OF, 2 - SF, 1 - CF, 0 - ZF
unsigned short ADDR, TEMP_ADDR;
unsigned char BUS, CONTROL;
int IOM = 0, RW = 0, OE = 0;

unsigned char twosComp(unsigned char op);
void printBin(int data, unsigned char data_width);
int ALU(void);
int multiplication(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void initMemory();
void MainMemory();
void IOMemory();
void setFlags(int ACC);
void printFlags();
void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand,
                   unsigned short MBR, unsigned char BUS, unsigned short ADDR, unsigned short MAR,
                   unsigned short IO_AR, unsigned char IO, unsigned short IO_BR, unsigned char FETCH,
                   unsigned char MEMORY, unsigned char CONTROL, int IOM, int RW, int OE);
int CU(void);

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

unsigned char twosComp(unsigned char op)
{
    return ~op + 1;
}

void printBin(int data, unsigned char data_width)
{
    for (int x = (int)data_width - 1; x >= 0; --x)
    {
        putchar((data & (1 << x)) ? '1' : '0');
    }
}

void printACC(int ACC)
{
    printf("ACC: ");
    printBin((int)ACC, 8);
    printFlags();
}

int ALU(void)
{
    static int ACC = 0x0000;
    unsigned char temp_OP2;
    unsigned int temp_ACC;
    printf("------------------------------- ALU -------------------------------\n");
    if (CONTROL == 0x1E || CONTROL == 0x1D) // 1E ADD, 1D SUB
    {
        if (CONTROL == 0x1D)
        {
            temp_OP2 = twosComp(BUS);
        }
        else
        {
            temp_OP2 = BUS;
        }
        temp_ACC = (int)ACC + temp_OP2;
        temp_ACC = (unsigned char) temp_ACC;
        setFlags(temp_ACC);
        if(ACC < BUS && CONTROL == 0x1D) {
            FLAGS |= 0x02;
        }
        printACC(temp_ACC);
        ACC = (unsigned char)temp_ACC;
    }
    else if (CONTROL == 0x1B) // MULTIPLICATION
    {

        temp_OP2 = BUS;
        temp_ACC = (int)ACC * temp_OP2;
        setFlags(temp_ACC); // using temp_ACC for seting flag as ACC is truncated therefore OF and CF are not triggered.
        ACC = (unsigned char)temp_ACC;
        printACC(ACC);
    }
    else if (CONTROL == 0x1A) // AND OPERATION
    {
        temp_OP2 = BUS;
        ACC = ACC & temp_OP2;
        printACC(ACC);
    }
    else if (CONTROL == 0x19) // OR OPERATION
    {
        temp_OP2 = BUS;
        ACC = ACC | temp_OP2;
        printACC(ACC);
    }
    else if (CONTROL == 0x18) // NOT OPERATION
    {
        ACC = ~ACC;
        printACC(ACC);
    }
    else if (CONTROL == 0x17) // XOR OPERATION
    {
        temp_OP2 = BUS;
        ACC = ACC ^ temp_OP2;
        printACC(ACC);
    }
    else if (CONTROL == 0x16) // SHL OPERATION
    {
        ACC = ACC << 1;
        printACC(ACC);
    }
    else if (CONTROL == 0x15) // SHR OPERATION
    {
        ACC = ACC >> 1;
        printACC(ACC);
    }
    else if (CONTROL == 0x09) // WACC OPERATION
    {
        ACC = BUS;
        printACC(ACC);
    }
    else if (CONTROL == 0x0B) // RACC OPERATION
    {
        BUS = ACC;
        printf("BUS: ");
        printBin((int)BUS, 16);
        printf("\n");
        printf("ACC\t\t: 0x%02X", ACC);
    }
    else if (CONTROL == 0x11) // BRLT OPERATION
    {
        temp_OP2 = twosComp(BUS);
        temp_ACC = (int)ACC + temp_OP2;
        temp_ACC = (unsigned char) temp_ACC;
        setFlags(temp_ACC);
        if(ACC < BUS) {
            FLAGS |= 0x02;
        }
        printACC(temp_ACC);
        printf("ACC\t\t: 0x%02X", temp_ACC);
        ACC = (unsigned char)temp_ACC;
    }
    else if (CONTROL == 0x12) // BRGT OPERATION
    {
        temp_OP2 = twosComp(BUS);
        temp_ACC = (int)ACC + temp_OP2;
        temp_ACC = (unsigned char) temp_ACC;
        setFlags(temp_ACC);
        if(ACC < BUS) {
            FLAGS |= 0x02;
        }
        printACC(temp_ACC);
        printf("ACC\t\t: 0x%02X", temp_ACC);
        ACC = (unsigned char)temp_ACC;
    }
    else if (CONTROL == 0x13) // BRNE OPERATION
    {
        temp_OP2 = twosComp(BUS);
        temp_ACC = (int)ACC + temp_OP2;
        temp_ACC = (unsigned char) temp_ACC;
        setFlags(temp_ACC);
        if(ACC < BUS) {
            FLAGS |= 0x02;
        }
        printACC(temp_ACC);
        printf("ACC\t\t: 0x%02X", temp_ACC);
        ACC = (unsigned char)temp_ACC;
    }
    else if (CONTROL == 0x14) // BRE OPERATION
    {
        temp_OP2 = twosComp(BUS);
        temp_ACC = (int)ACC + temp_OP2;
        temp_ACC = (unsigned char) temp_ACC;
        setFlags(temp_ACC);
        if(ACC < BUS) {
            FLAGS |= 0x02;
        }
        printACC(temp_ACC);
        printf("ACC\t\t: 0x%02X", temp_ACC);
        ACC = (unsigned char)temp_ACC;
    }
    else
    {
        printf("Error!");
    }
    return 0;
}

int CU(void)
{
    unsigned short operand, IR, PC, MAR = 0, MBR, IO_AR, IO_BR = 0, inst_code;
    unsigned char progMemoryStart = 0x000;
    unsigned short dataMemoryStart = 0x400;
    unsigned char FETCH, IO, MEMORY;
    int temp = 0;

    PC = progMemoryStart;
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

        TEMP_ADDR = ADDR - 1;

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
            ADDR = PC - 1;
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
        else if (inst_code == 0x09)
        { // Write data on BUS to ACC (WACC)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x0B)
        { // Read ACC to bus (RACC)
            CONTROL = inst_code;
            OE = 0;
            temp = ALU();
            MBR = BUS;
        }
        else if (inst_code == 0x0E)
        { // Swap data of MBR and IOBR (SWAP)
            CONTROL = inst_code;
            OE = 0;
            temp = MBR;
            MBR = IO_BR;
            IO_BR = temp;
        }
        else if (inst_code == 0x1E)
        { // Adds the data on the BUS to ACC register, sum stored to ACC (ADD)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x1D)
        { // Subtract the data on the BUS from the ACC register, difference stored to ACC (SUB)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x1B)
        { // Multiply the value of ACC to BUS, product stored to ACC (MUL)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x1A)
        { // AND the value of ACC and BUS, result stored to ACC
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x19)
        { // OR the value of ACC and BUS, result stored to ACC (OR)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x18)
        { // Complement the value of ACC, result stored to ACC (NOT)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x17)
        { // XOR the value of ACC and BUS, result stored to ACC (XOR)
            CONTROL = inst_code;
            OE = 0;
            BUS = MBR;
            temp = ALU();
        }
        else if (inst_code == 0x16)
        {
            CONTROL = inst_code;
            temp = ALU();
        }
        else if (inst_code == 0x15)
        {
            CONTROL = inst_code;
            temp = ALU();
        }
        else if (inst_code == 0x14)
        { // Compare ACC and BUS, if equal branch to address specified (BRE)
            CONTROL = inst_code;
            ADDR = operand;
            OE = 0;
            BUS = MBR;
            temp = ALU();
            if ((FLAGS & 0x01) == 0x01)
            {
                PC = ADDR;
            }
        }
        else if (inst_code == 0x13)
        { // Compare ACC and BUS, if not equal branch to address specified (BRNE)
            CONTROL = inst_code;
            ADDR = operand;
            OE = 0;
            BUS = MBR;
            temp = ALU();
            if ((FLAGS & 0x01) == 0x00)
            {
                PC = ADDR;
            }
        }
        else if (inst_code == 0x12)
        { // Compare ACC and BUS, if ACC > BUS, branch to address specified (BRGT)
            CONTROL = inst_code;
            ADDR = operand;
            OE = 0;
            BUS = MBR;
            temp = ALU();
            if ((FLAGS & 0x02) == 0)
            {
                PC = ADDR;
            }
        }
        else if (inst_code == 0x11)
        { // Compare ACC and BUS, if ACC < BUS, branch to address specified (BRLT)
            CONTROL = inst_code;
            ADDR = operand;
            OE = 0;
            BUS = MBR;
            temp = ALU();
            if (((FLAGS & 0x02) >> 1) == 0x01)
            {
                PC = ADDR;
            }
        }
        else if (inst_code != 0x1F)
        {
            return 0;
        }
        outputDisplay(IR, inst_code, operand, MBR, BUS, ADDR, MAR, IO_AR, IO, IO_BR, FETCH, MEMORY, CONTROL, IOM, RW, OE);
    }
    return 1;
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

void outputDisplay(unsigned short IR, unsigned short inst_code, unsigned short operand,
                   unsigned short MBR, unsigned char BUS, unsigned short ADDR, unsigned short MAR,
                   unsigned short IO_AR, unsigned char IO, unsigned short IO_BR, unsigned char FETCH,
                   unsigned char MEMORY, unsigned char CONTROL, int IOM, int RW, int OE)
{
    printf("\n===================================================================\n");
    printf("Address BUS\t: 0x%04X\n", TEMP_ADDR);
    printf("DATA BUS\t: 0x%02X\n\n", BUS);
    printf("MAR \t\t: 0x%04X\n", MAR);
    printf("MBR \t\t: 0x%02X\n", MBR);
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

    printf("IR \t \t: 0x%04X\n", IR);
    printf("Instruction Code: 0x%02X\n", inst_code);
    printf("Operand \t: 0x%04X\n", operand);
    if (inst_code == 0x01)
    {
        printf("Instruction \t: WM\n");
        printf("Writing data to memory...\n");
        printf("MBR \t\t: 0x%02X\n", MBR);
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
    else if (inst_code == 0x1E)
    {
        printf("Instruction\t: ADD\n");
        printf("Added BUS to ACC...\n");
    }
    else if (inst_code == 0x1D)
    {
        printf("Instruction\t: SUB\n");
        printf("Subtracted ACC from BUS...\n");
    }
    else if (inst_code == 0x1B)
    {
        printf("Instruction\t: MUL\n");
        printf("Multiplied ACC from BUS...\n");
    }
    else if (inst_code == 0x1A)
    {
        printf("Instruction\t: AND\n");
        printf("AND Operation for ACC and BUS...\n");
    }
    else if (inst_code == 0x19)
    {
        printf("Instruction\t: OR\n");
        printf("OR Operation for ACC and BUS...\n");
    }
    else if (inst_code == 0x18)
    {
        printf("Instruction\t: NOT\n");
        printf("NOT Operation of ACC...\n");
    }
    else if (inst_code == 0x17)
    {
        printf("Instruction\t: XOR\n");
        printf("XOR Operation of ACC and BUS...\n");
    }
    else if (inst_code == 0x16)
    {
        printf("Instruction\t: SHL\n");
        printf("Shift left ACC by 1...\n");
    }
    else if (inst_code == 0x15)
    {
        printf("Instruction\t: SHR\n");
        printf("Shift right ACC by 1...\n");
    }
    else if (inst_code == 0x0B)
    {
        printf("Instruction\t: RACC\n");
        printf("Read ACC to BUS...\n");
    }
    else if (inst_code == 0x0E)
    {
        printf("Instruction\t: SWAP\n");
        printf("Swap data of MBR and IOBR...\n");
    }
    else if (inst_code == 0x14)
    {
        printf("Instruction\t: BRE\n");
        printf("ACC and BUS equal, branching to address...\n");
    }
    else if (inst_code == 0x13)
    {
        printf("Instruction\t: BRNE\n");
        printf("ACC and BUS not equal, branching to address...\n");
    }
    else if (inst_code == 0x12)
    {
        printf("Instruction\t: BRGT\n");
        printf("ACC greater than BUS, branching to address...\n");
    }
    else if (inst_code == 0x11)
    {
        printf("Instruction\t: BRLT\n");
        printf("ACC lesser than BUS, branching to address...\n");
    }
    else if (inst_code == 0x09)
    {
        printf("Instruction\t: WACC\n");
        printf("Writing data on BUS to ACC...\n");
    }

    printf("===================================================================\n\n");
}

void setFlags(int ACC)
{
    FLAGS = 0x00;
    if (ACC == 0x0000) // ZF
    {
        FLAGS |= 0x01;
    }
    if (ACC > 0xFF) // OF
    {
        FLAGS |= 0x80;
    }
    if (ACC > 0xFF) // CF
    {
        FLAGS |= 0x02;
    }
}

void printFlags()
{
    printf("\nZF=%d, CF=%d, SF=%d, OF=%d\n", (FLAGS & 0x01), (FLAGS & 0x02) >> 1, (FLAGS & 0x04) >> 2, (FLAGS & 0x80) >> 7);
}

void initMemory(void)
{
    printf("Initializing Main Memory...\n\n");
    IOM = 1, RW = 1, OE = 1;

    // (WB) MBR <- 0x15
    ADDR = 0x000;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x001;
    BUS = 0x15;
    MainMemory();

    // (WM) 0x400 <- MBR
    ADDR = 0x002;
    BUS = 0x0C;
    MainMemory();
    ADDR = 0x003;
    BUS = 0x00;
    MainMemory();

    // (WB) MBR <- 0x05
    ADDR = 0x004;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x005;
    BUS = 0x05;
    MainMemory();

    // (WACC) ACC <- BUS
    ADDR = 0x006;
    BUS = 0x48;
    MainMemory();
    ADDR = 0x007;
    BUS = 0x00;
    MainMemory();

    // (WB) MBR <- 0x08
    ADDR = 0x008;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x009;
    BUS = 0x08;
    MainMemory();

    // (ADD) ACC + BUS
    ADDR = 0x00A;
    BUS = 0xF0;
    MainMemory();
    ADDR = 0x00B;
    BUS = 0x00;
    MainMemory();

    // (RM) MBR <- 0x400
    ADDR = 0x00C;
    BUS = 0x14;
    MainMemory();
    ADDR = 0x00D;
    BUS = 0x00;
    MainMemory();

    // (MUL) ACC * BUS
    ADDR = 0x00E;
    BUS = 0xD8;
    MainMemory();
    ADDR = 0x00F;
    BUS = 0x00;
    MainMemory();

    // (RACC) BUS <- ACC
    ADDR = 0x010;
    BUS = 0x58;
    MainMemory();
    ADDR = 0x011;
    BUS = 0x00;
    MainMemory();

    // (WM) 0x401 <- MBR
    ADDR = 0x012;
    BUS = 0x0C;
    MainMemory();
    ADDR = 0x013;
    BUS = 0x01;
    MainMemory();

    // (WIB) IOBR <- 0x0B
    ADDR = 0x014;
    BUS = 0x38;
    MainMemory();
    ADDR = 0x015;
    BUS = 0x0B;
    MainMemory();

    // (WIO) 0x000 <- IOBR
    ADDR = 0x016;
    BUS = 0x28;
    MainMemory();
    ADDR = 0x017;
    BUS = 0x00;
    MainMemory();

    // (WB) MBR <- 0x10
    ADDR = 0x018;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x019;
    BUS = 0x10;
    MainMemory();

    // (SUB) ACC - BUS
    ADDR = 0x01A;
    BUS = 0xE8;
    MainMemory();
    ADDR = 0x01B;
    BUS = 0x00;
    MainMemory();

    // (RACC) BUS <- ACC
    ADDR = 0x01C;
    BUS = 0x58;
    MainMemory();
    ADDR = 0x01D;
    BUS = 0x00;
    MainMemory();

    // (WIO) 0x001 <- IOBR
    ADDR = 0x01E;
    BUS = 0x28;
    MainMemory();
    ADDR = 0x01F;
    BUS = 0x01;
    MainMemory();

    // (SHL) ACC << 1, CF <- ACC(7)
    ADDR = 0x020;
    BUS = 0xB0;
    MainMemory();
    ADDR = 0x021;
    BUS = 0x00;
    MainMemory();

    // (SHL) ACC << 1, CF <- ACC(7)
    ADDR = 0x022;
    BUS = 0xB0;
    MainMemory();
    ADDR = 0x023;
    BUS = 0x00;
    MainMemory();

    // (SHR) ACC >> 1, CF <- ACC(0)
    ADDR = 0x024;
    BUS = 0xA8;
    MainMemory();
    ADDR = 0x025;
    BUS = 0x00;
    MainMemory();

    // (RM) MBR <- 0x401
    ADDR = 0x026;
    BUS = 0x14;
    MainMemory();
    ADDR = 0x027;
    BUS = 0x01;
    MainMemory();

    // (OR) ACC | BUS
    ADDR = 0x028;
    BUS = 0xC8;
    MainMemory();
    ADDR = 0x029;
    BUS = 0x00;
    MainMemory();

    // (NOT) !ACC
    ADDR = 0x02A;
    BUS = 0xC0;
    MainMemory();
    ADDR = 0x02B;
    BUS = 0x00;
    MainMemory();

    // (RIO) IOBR <- 0x001
    ADDR = 0x02C;
    BUS = 0x20;
    MainMemory();
    ADDR = 0x02D;
    BUS = 0x01;
    MainMemory();

    // (SWAP) MBR <-> IOBR
    ADDR = 0x02E;
    BUS = 0x70;
    MainMemory();
    ADDR = 0x02F;
    BUS = 0x00;
    MainMemory();

    // (XOR) ACC ^ BUS
    ADDR = 0x030;
    BUS = 0xB8;
    MainMemory();
    ADDR = 0x031;
    BUS = 0x00;
    MainMemory();

    // (WB) MBR <- 0xFF
    ADDR = 0x032;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x033;
    BUS = 0xFF;
    MainMemory();

    // (AND) ACC & BUS
    ADDR = 0x034;
    BUS = 0xD0;
    MainMemory();
    ADDR = 0x035;
    BUS = 0x00;
    MainMemory();

    // (RM) MBR <- 0x401
    ADDR = 0x036;
    BUS = 0x14;
    MainMemory();
    ADDR = 0x037;
    BUS = 0x01;
    MainMemory();

    // (BRE) PC <- 0x03C if ACC - BUS == 0
    ADDR = 0x038;
    BUS = 0xA0;
    MainMemory();
    ADDR = 0x039;
    BUS = 0x3C;
    MainMemory();

    // (WM) 0xF0 <- MBR // WEIRD OPERATION, IDK if TYPO or what????
    ADDR = 0x03A;
    BUS = 0x30;   // (WM) is shown! but test case show that MBR = 0xF0
    MainMemory(); // will follow result of test case using the appropriate operation (WB)
    ADDR = 0x03B;
    BUS = 0xF0;
    MainMemory();

    // (BRGT) PC <- 0x040 if ACC - BUS > 0
    ADDR = 0x03C;
    BUS = 0x90;
    MainMemory();
    ADDR = 0x03D;
    BUS = 0x40;
    MainMemory();

    // (BRLT) PC <- 0x044 if ACC - BUS < 0
    ADDR = 0x03E;
    BUS = 0x88;
    MainMemory();
    ADDR = 0x03F;
    BUS = 0x44;
    MainMemory();

    // (WB) MBR <- 0x00
    ADDR = 0x040;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x041;
    BUS = 0x00; // 0x00 original
    MainMemory();

    // (WACC) ACC <- BUS
    ADDR = 0x042;
    BUS = 0x48;
    MainMemory();
    ADDR = 0x043;
    BUS = 0x00; // 0x00
    MainMemory();

    // (WB) MBR <- 0x03
    ADDR = 0x044;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x045;
    BUS = 0x03;
    MainMemory();

    // (WACC) ACC <- BUS
    ADDR = 0x046;
    BUS = 0x48;
    MainMemory();
    ADDR = 0x047;
    BUS = 0x00;
    MainMemory();

    // (WB) MBR <- 0x00
    ADDR = 0x048;
    BUS = 0x30;
    MainMemory();
    ADDR = 0x049;
    BUS = 0x00;
    MainMemory();

    // (BRE) PC <- 0x052 if ACC - BUS == 0 //NEEDS CHECKING HERE!!!
    ADDR = 0x04A;
    BUS = 0xA0;
    MainMemory();
    ADDR = 0x04B;
    BUS = 0x52;
    MainMemory();

    // (WB) MBR <- 0x01
    ADDR = 0x04C;
    BUS = 0x30;
    MainMemory(); // WB
    ADDR = 0x04D;
    BUS = 0x01;
    MainMemory();

    // (SUB) ACC - BUS
    ADDR = 0x04E;
    BUS = 0xE8;
    MainMemory();
    ADDR = 0x04F;
    BUS = 0x00;
    MainMemory();

    // (BR) PC <- 0x048
    ADDR = 0x050;
    BUS = 0x18;
    MainMemory();
    ADDR = 0x051;
    BUS = 0x48;
    MainMemory();

    // EOP
    ADDR = 0x052;
    BUS = 0xF8;
    MainMemory();
    ADDR = 0x053;
    BUS = 0x00;
    MainMemory();
}
