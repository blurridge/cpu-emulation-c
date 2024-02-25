#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int addition(int op1, int op2)
{
    int sum = op1 + op2;
    return sum;
}

int NOT(int op)
{
    return ~op + 1; // need to +1 because bogo ang NOT for some reason
}

int subtraction(int op1, int op2)
{
    int temp = NOT(op2);
    int sum = addition(op1, temp);
    return sum;
}

int multiplication(unsigned char op1, unsigned char op2)
{
    int ACC;          // Product
    int Qn1;          // least significant bit Qn+1
    unsigned char BR; // Multipicand - A
    int QR;           // Multiplier - M
    int SC;           // Number of Rows - n
    BR = op1;

    printBin((int)BR, 0x08);

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
    int result = 0;
    if (control_signals == 0x01)
    {
        result = addition((int)operand1, (int)operand2);
    }
    else if (control_signals == 0x02)
    {
        result = subtraction((int)operand1, (int)operand2);
    }
    else if (control_signals == 0x03)
    {
        result = multiplication((int)operand1, (int)operand2);
    }
    return result;
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

void delay(int seconds)
{
    sleep(seconds);
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

int main()
{
    int result;
    printf("First Input: ");
    unsigned char temp_operand1;
    scanf("%i", &temp_operand1);
    printf("First Input: >", temp_operand1);
    unsigned char operand1 = (unsigned char)temp_operand1;

    printf("\nSecond Input: ");
    unsigned int temp_operand2;
    scanf("%i", &temp_operand2);
    unsigned char operand2 = (unsigned char)temp_operand2;

    printf("\nOperation: ");
    unsigned int temp_control_signals;
    scanf("%i", &temp_control_signals);
    unsigned char control_signals = (unsigned char)temp_control_signals;

    result = ALU(operand1, operand2, control_signals);

    output_display((int)operand1, (int)operand2, control_signals, result);

    return 0;
}