#include <stdio.h>

int addition(int op1, int op2)
{
    int sum = op1 + op2;
    return sum;
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    int result = 0;
    if (control_signals == 0x01)
    {
        result = addition((int)operand1, (int)operand2);
        return result;
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
        printf("Substraction");
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

void delay(int cnt)
{
    int i;
    for (i = 0; i <= cnt; i++)
    {
    }
}

void output_display(int opr1, int opr2, unsigned char control_signals, int result)
{
    int x = 1000000;
    printf("\nFetching operands....");
    delay(x);

    printf("\nOP1 = ");
    printBin(opr1, 0x08);

    printf("\nOP2  = ");
    printBin(opr2, 0x08);

    printf("\nOperation = ");
    display_controlSignalType(control_signals);

    printf("\nProcessing OP1 & OP2....");
    delay(x);

    printf("\nResult = ");
    printBin(result, 0x08);
}

int main()
{
    int result;
    printf("First Input: ");
    unsigned char operand1;
    scanf("%hhx", &operand1);

    printf("\nSecond Input: ");
    unsigned char operand2;
    scanf("%hhx", &operand2);

    printf("\nOperation: ");
    unsigned char control_signals;
    scanf("%hhx", &control_signals);

    result = ALU(operand1, operand2, control_signals);
    output_display((int)operand1, (int)operand2, control_signals, result);

    return 0;
}
