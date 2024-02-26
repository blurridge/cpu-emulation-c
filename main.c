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

    if(controlSignals < 0x01 || controlSignals > 0x09) 
    {
        printf("Not a valid operation.");
        return 0;
    }

    printf("First Input: ");
    scanf("%hhx", &operand1);

    if(controlSignals != 0x06 && controlSignals != 0x08 && controlSignals != 0x09)
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
        printf("Addition");
    }
    else if (control_signals == 0x02)
    {
        printf("Subtraction");
    }
    else if (control_signals == 0x03)
    {
        printf("Multiplication");
    }
    else if (control_signals == 0x04)
    {
        printf("AND Bitwise");
    }
    else if (control_signals == 0x05)
    {
        printf("OR Bitwise");
    }
    else if (control_signals == 0x06)
    {
        printf("NOT Bitwise");
    }
    else if (control_signals == 0x07)
    {
        printf("XOR Bitwise");
    }
    else if (control_signals == 0x08)
    {
        printf("Shift Right (Logical)");
    }
    else if (control_signals == 0x09)
    {
        printf("Shift Left (Logical)");
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
    unsigned short int ACC; // Product
    unsigned char Qn1;      // least significant bit Qn+1
    unsigned char BR;       // Multipicand - A
    unsigned char Q;        // Q

    printf("\n    A        Q    Qn1     M    n\n");

    int current_operation = 0;
    Q = operand2;
    BR = 0x00;
    unsigned char LSB_A; // Multipicand - A
    unsigned char LSB_Q;

    unsigned char MSB_M = (operand1 >> 7) & 0x01;

    for (int i = 0; i <= 8; i++)
    {
        LSB_A = BR & 0x01;
        LSB_Q = Q & 0x01;
        if (i == 0)
        {
            Qn1 = 0x00;
            printBin((int)BR, 0x08);
            printf(" ");
            printBin((int)Q, 0x08);
            printf("  %x  ", Qn1);
            printBin((int)operand1, 0x08);
            printf(" %d\n", i);
        }
        else
        {
            if (((Q & 0x01) == 0x01) && (Qn1 == 0x01))
            {
                BR = BR >> 1;
                Q = Q >> 1;

                Q = Q | (LSB_A << 7);
                Qn1 = LSB_Q;

                if (current_operation == 1)
                {
                    BR = BR | 0x80;
                }
            }

            else if (((Q & 0x01) == 0x00) && (Qn1 == 0x00))
            {
                BR = BR >> 1;
                Q = Q >> 1;

                Q = Q | (LSB_A << 7);
                Qn1 = LSB_Q;

                if (current_operation == 1)
                {
                    BR = BR | 0x80;
                }
            }

            else if (((Q & 0x01) == 0x01) && (Qn1 == 0x00))
            {
                BR = subtraction(BR, operand1);

                LSB_A = BR & 0x01;
                LSB_Q = Q & 0x01;

                BR = BR >> 1;
                Q = Q >> 1;

                Q = Q | (LSB_A << 7);
                Qn1 = LSB_Q;
                if (MSB_M == 0x01)
                {
                    current_operation = 0;
                }
                else
                {
                    current_operation = 1;
                    BR = BR | 0x80;
                }
            }
            else if (((Q & 0x01) == 0x00) && (Qn1 == 0x01))
            {
                BR = BR + operand1;

                LSB_A = BR & 0x01;
                LSB_Q = Q & 0x01;

                BR = BR >> 1;
                Q = Q >> 1;

                Q = Q | (LSB_A << 7);
                Qn1 = LSB_Q;
                if (MSB_M == 0x01)
                {
                    current_operation = 1;
                    BR = BR | 0x80;
                }
                else
                {
                    current_operation = 0;
                }
            }
            printBin((int)BR, 0x08);
            printf(" ");
            printBin((int)Q, 0x08);
            printf("  %x  ", Qn1);
            printBin((int)operand1, 0x08);
            printf(" %d\n", i);
        }
    }

    printf("\n\nProcessing OP1 & OP2....");
    ACC = (BR << 8) | Q;
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

void printOperands(unsigned char operand1, unsigned char operand2, unsigned char controlSignals) {
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
    if(controlSignals == 0x03)
        printBin(result, 0x10);
    else
        printBin(result, 0x08);
    printFlags();
    printf("\n==============================================");
}

void printFlags() 
{
    printf("\nZF=%d, CF=%d, SF=%d, OF=%d", ZF, CF, SF, OF);
}