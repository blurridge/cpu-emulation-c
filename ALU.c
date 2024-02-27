/*============================================================================================================
FILENAME        :ALU.c
DESCRIPTION     :Create a program that will display the 1-N value of the Fibonacci sequence. Use a space for 
                the spacing between numbers horizontally.
AUTHOR          :Zach Riane I. Machacon, Eldridge Aaron S. Miole, Dave Nelson F. Ogue, John Jason C. Zamora
CREATED ON      :27 February 2024
============================================================================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

bool SF, OF, ZF, CF;

unsigned char NOT(unsigned char op);
unsigned char AND(unsigned char op1, unsigned char op2);
unsigned char OR(unsigned char op1, unsigned char op2);
unsigned char XOR(unsigned char op1, unsigned char op2);
unsigned char shiftRight(unsigned char op1);
unsigned char shiftLeft(unsigned char op1);
void printBin(int data, unsigned char data_width);
void delay(int seconds);
void displayControlSignalType(unsigned char control_signals);
void setFlag(unsigned int ACC);
int addition(unsigned char op1, unsigned char op2);
int subtraction(unsigned char op1, unsigned char op2);
int multiplication(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
unsigned char twosComp(unsigned char op);
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void printOperands(unsigned char operand1, unsigned char operand2, unsigned char controlSignals);
void outputDisplay(unsigned char control_signals, int result);
void printFlags();

int main()
{
    unsigned int result;
    unsigned char operand1, controlSignals;
    unsigned char operand2 = 0x00;

    printf("\nOperation: ");
    scanf("%hhx", &controlSignals);

    if (controlSignals < 0x01 || controlSignals > 0x09)
    {
        printf("Not a valid operation.");
        return 0;
    }

    printf("First Input: ");
    scanf("%hhx", &operand1);

    if (controlSignals != 0x06 && controlSignals != 0x08 && controlSignals != 0x09)
    {
        printf("Second Input: ");
        scanf("%hhx", &operand2);
    }

    printOperands(operand1, operand2, controlSignals);
    result = ALU(operand1, operand2, controlSignals);
    outputDisplay(controlSignals, result);

    return 0;
}

void delay(int seconds)
{
    sleep(seconds);
}

void displayControlSignalType(unsigned char control_signals)
{
    if (control_signals == 0x01)
    {
        printf("ADD");
    }
    else if (control_signals == 0x02)
    {
        printf("SUB");
    }
    else if (control_signals == 0x03)
    {
        printf("MUL");
    }
    else if (control_signals == 0x04)
    {
        printf("AND");
    }
    else if (control_signals == 0x05)
    {
        printf("OR");
    }
    else if (control_signals == 0x06)
    {
        printf("NOT");
    }
    else if (control_signals == 0x07)
    {
        printf("XOR");
    }
    else if (control_signals == 0x08)
    {
        printf("SHR");
    }
    else if (control_signals == 0x09)
    {
        printf("SHL");
    }
}

void setFlag(unsigned int ACC)
{
    if (ACC == 0x0000)
    {
        ZF = 1;
    }
    if ((ACC & 0x0080) == 0x0080)
    {
        SF = 1;
    }
    if (ACC > 0X7F)
    {
        OF = 1;
    }
    if (ACC > 0xFF)
    {
        CF = 1;
    }
}

unsigned char NOT(unsigned char op)
{
    return ~op;
}

unsigned char AND(unsigned char op1, unsigned char op2)
{
    return op1 & op2;
}

unsigned char OR(unsigned char op1, unsigned char op2)
{
    return op1 | op2;
}

unsigned char XOR(unsigned char op1, unsigned char op2)
{
    return op1 ^ op2;
}

unsigned char twosComp(unsigned char op)
{
    return ~op + 1;
}

int addition(unsigned char op1, unsigned char op2)
{
    if ((op1 & 0x80) == 0x80)
    {
        op1 = twosComp(op1);
    }
    else if ((op1 & 0x80) == 0x80)
    {
        op2 = twosComp(op2);
    }
    unsigned int sum = op1 + op2;
    return sum;
}

int subtraction(unsigned char op1, unsigned char op2)
{
    op2 = twosComp(op2);
    unsigned int difference = op1 + op2;
    return difference;
}

int multiplication(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    unsigned char Qn1 = 0x00;   // least significant bit Qn+1
    unsigned char A = 0x00;     // Multiplicand - A
    unsigned char Q = operand2; // Q

    printf("\n    A        Q    Qn1     M    n\n");

    for (int i = 0; i <= 8; i++)
    {
        printBin((int)A, 0x08);
        printf(" ");
        printBin((int)Q, 0x08);
        printf("  %x  ", Qn1);
        printBin((int)operand1, 0x08);
        printf(" %d\n", i);

        if (((Q & 0x01) == 0x01) && (Qn1 == 0x00)) // {Q,Qn1} = 10
        {
            A = subtraction(A, operand1);
        }
        else if (((Q & 0x01) == 0x00) && (Qn1 == 0x01)) // {Q,Qn1} = 01
        {
            A = A + operand1;
        }

        unsigned char MSB_A = (A >> 7) & 0x01;
        unsigned char LSB_A = A & 0x01;
        unsigned char LSB_Q = Q & 0x01;

        A = A >> 1; // Shifting A
        Q = Q >> 1; // Shifting Q

        Q = Q | (LSB_A << 7); // LSM of A is shifted to the MSB of Q

        Qn1 = LSB_Q; // Store LSB of Q to Qn1 for conditions

        A = A | (MSB_A << 7); // Either Shift 1 or 0 to the MSB of A
    }

    unsigned short int ACC = (A << 8) | Q;
    return ACC;
}

unsigned char shiftRight(unsigned char op1)
{
    return op1 >> 1;
}

unsigned char shiftLeft(unsigned char op1)
{
    return op1 << 1;
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    unsigned int ACC = 0x0000;
    SF = 0, CF = 0, ZF = 0, OF = 0;
    if (control_signals == 0x01)
    {
        ACC = addition(operand1, operand2);
    }
    else if (control_signals == 0x02)
    {
        ACC = subtraction(operand1, operand2);
    }
    else if (control_signals == 0x03)
    {
        ACC = multiplication(operand1, operand2, control_signals);
    }
    else if (control_signals == 0x04)
    {
        ACC = AND(operand1, operand2);
    }
    else if (control_signals == 0x05)
    {
        ACC = OR(operand1, operand2);
    }
    else if (control_signals == 0x06)
    {
        ACC = NOT(operand1);
    }
    else if (control_signals == 0x07)
    {
        ACC = XOR(operand1, operand2);
    }
    else if (control_signals == 0x08)
    {
        ACC = shiftRight(operand1);
    }
    else if (control_signals == 0x09)
    {
        ACC = shiftLeft(operand1);
    }
    setFlag(ACC);
    return ACC;
}

void printBin(int data, unsigned char data_width)
{
    unsigned long sum = 0;
    unsigned long power = 1;
    while (data)
    {
        if (data & 1)
        {
            sum += power;
        }
        power *= 10;
        data /= 2;
    }
    printf("%0*lu", data_width, sum);
}

void printOperands(unsigned char operand1, unsigned char operand2, unsigned char controlSignals)
{
    int x = 3;
    int y = 2;
    printf("\n==============================================\n");
    printf("Fetching operands....");
    delay(x);

    printf("\nOP1 = ");
    printBin((int)operand1, 0x08);
    delay(y);

    printf("\nOP2  = ");
    printBin((int)operand2, 0x08);
    delay(y);

    printf("\nOperation = ");
    displayControlSignalType(controlSignals);

    printf("\n\nProcessing OP1 & OP2....");
    delay(x);
}

void outputDisplay(unsigned char controlSignals, int result)
{
    printf("\nAccumulator = ");
    printBin(result, 0x10);
    printFlags();
    printf("\n==============================================");
}

void printFlags()
{
    printf("\nZF=%d, CF=%d, SF=%d, OF=%d", ZF, CF, SF, OF);
}