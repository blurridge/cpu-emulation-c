#include <stdio.h>
#include <unistd.h>

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

void delay(int seconds) {
    sleep(seconds);
}

void output_display(unsigned char operand1, unsigned char operand2, unsigned char control_signals, int result)
{
    int x = 3;
    printf("\nFetching operands....");
    delay(x);

    printf("\nOP1 = ");
    printBin((int)operand1, 0x08);

    printf("\nOP2  = ");
    printBin((int)operand2, 0x08);

    printf("\n\nOperation = ");
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
    unsigned int temp_operand1;
    scanf("%i", &temp_operand1);
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