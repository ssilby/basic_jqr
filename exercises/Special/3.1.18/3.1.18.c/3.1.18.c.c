#include <stdio.h>
#include <stdlib.h>

int addition(int *firstNum, int *secondNum);

int main(int argc, char *argv[])
{
    int returnValue = 0;
    int numA = 91;
    int numB = 9;

    if (argc != 1 && argc!=3)
    {
        printf("Command only takes 0 or two arguments, both must be intigers.");
    }

    if (argc == 3 )
    {
        numA = atoi(argv[1]);
        numB = atoi(argv[2]);
    }

    printf("This program is cross-compiled for the ARMv8 architecture\n");


    returnValue = addition(&numA, &numB);

    printf("Math works on an ARMv8 as well %i + %i = %i : \n",
            numA, numB, returnValue);

    exit(0);
}

int addition(int *firstNum, int *secondNum)
{
    return *firstNum + *secondNum;
}
