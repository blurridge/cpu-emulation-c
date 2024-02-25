#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

bool SF, OF, ZF, CF;

void setFlag(unsigned int ACC){
    if (ACC == 0x0000){
        ZF = 1;
    }
    else if (ACC & 0x0080 == 0x0080){
        SF = 1;
    }
    else if (ACC > 0X7F){
        OF = 1;
    }
    else if (ACC > 0xFF){
        CF = 1;
    }
}

int addition(unsigned char op1, unsigned char op2)
{
    unsigned int sum = op1 + op2;
    return sum;
}

int subtraction(unsigned char op1, unsigned char op2){
    unsigned char temp = NOT(op2);
    unsigned int sum = addition(op1, temp);
    return sum;
}

int NOT(int op){
    return ~op+1; //need to +1 because bogo ang NOT for some reason
}

unsigned char shiftRight(unsigned char op1){
    return op1 >> 1;
}

unsigned char shiftLeft(unsigned char op1){
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
    if (control_signals == 0x02)
    {
        ACC = subtraction(operand1, operand2);
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
    unsigned char temp_operand1;
    unsigned char temp_operand2;
    unsigned char temp_control_signals;
    printf("First Input: ");
    scanf("%i", &temp_operand1);
    unsigned char operand1 = temp_operand1;

    printf("\nSecond Input: ");
    scanf("%i", &temp_operand2);
    unsigned char operand2 = temp_operand2;

    printf("\nOperation: ");
    scanf("%i", &temp_control_signals);
    unsigned char control_signals = temp_control_signals;

    result = ALU(operand1, operand2, control_signals);
    output_display((int)operand1, (int)operand2, control_signals, result);

    return 0;
}