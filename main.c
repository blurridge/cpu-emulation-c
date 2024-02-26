#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

bool SF, OF, ZF, CF;

int NOT(int op);
void printBin(int data, unsigned char data_width);
void delay(int seconds);
void display_controlSignalType(unsigned char control_signals);
void setFlag(unsigned int ACC);
int addition(unsigned char op1, unsigned char op2);
int subtraction(unsigned char op1, unsigned char op2);
unsigned char twosComplement(unsigned char op);
int multiplication(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void printBinary(unsigned short int combined, unsigned char data_width);
unsigned char shiftRight(unsigned char op1);
unsigned char shiftLeft(unsigned char op1);
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void output_display(unsigned char operand1, unsigned char operand2, unsigned char control_signals, int result);

int main()
{
    unsigned int result;
    unsigned char temp_operand1;
    unsigned char temp_operand2;
    unsigned char temp_control_signals;
    printf("First Input: ");
    scanf("%hhx", &temp_operand1);
    unsigned char operand1 = temp_operand1;

    printf("\nSecond Input: ");
    scanf("%hhx", &temp_operand2);
    unsigned char operand2 = temp_operand2;

    printf("\nOperation: ");
    scanf("%hhx", &temp_control_signals);
    unsigned char control_signals = temp_control_signals;

    if (temp_control_signals == 0x03)
    {
        result = ALU(operand1, operand2, control_signals);
        printf("\nAccumulator = ");
        // printBin(result, 0x10);
        printBinary(result, 0x10); // Using the printBinary Function as printBin is not working properly in printing 16 bits of binary.
        printf("\n==============================================");
    }

    else
    {
        result = ALU(operand1, operand2, control_signals);
        output_display(operand1, operand2, control_signals, result);
    }

    return 0;
}

void delay(int seconds)
{
    sleep(seconds);
}

void display_controlSignalType(unsigned char control_signals)
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
}

void setFlag(unsigned int ACC)
{
    if (ACC == 0x0000)
    {
        ZF = 1;
    }
    else if ((ACC & 0x0080) == 0x0080)
    {
        SF = 1;
    }
    else if (ACC > 0X7F)
    {
        OF = 1;
    }
    else if (ACC > 0xFF)
    {
        CF = 1;
    }
}

int NOT(int op)
{
    return ~op;
}

unsigned char twosComplement(unsigned char op) 
{
    return ~op + 1;
}

int addition(unsigned char op1, unsigned char op2)
{
    unsigned int sum = op1 + op2;
    return sum;
}

int subtraction(unsigned char op1, unsigned char op2)
{
    unsigned char temp = twosComplement(op2);
    unsigned int sum = addition(op1, temp);
    return sum;
}

int multiplication(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    unsigned short int ACC; // Product
    unsigned char Qn1;      // least significant bit Qn+1
    unsigned char BR;       // Multipicand - A
    unsigned char Q;        // Q

    int x = 1;
    int y = 1;
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
    display_controlSignalType(control_signals);

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
                BR = addition(BR, operand1);

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
    delay(x);
    ACC = (BR << 8) | Q;
    return ACC;
}

void printBinary(unsigned short int combined, unsigned char data_width) // Another Print that converts to Binary, feel fre to discard if unnecessary
{
    for (int i = (data_width - 1); i >= 0; --i)
    {
        putchar((combined & (1 << i)) ? '1' : '0');
    }
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

void output_display(unsigned char operand1, unsigned char operand2, unsigned char control_signals, int result)
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
    display_controlSignalType(control_signals);

    printf("\n\nProcessing OP1 & OP2....");
    delay(x);

    printf("\nResult = ");
    printBin(result, 0x08);
    printf("\n==============================================");
}