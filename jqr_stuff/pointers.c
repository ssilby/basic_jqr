#include <stdio.h>

void print_me(void);
void print_functs(int, void (*f)(void));

int main(void)
{
    // Declaring an integer pointer
    // Dereferencing a variable to get its value
    int var = 50;
    int *int_pointer;
    //assign a value to a pointer
    int_pointer = &var;
    printf("value at int_pointer address is %d\n", *int_pointer);

    // Printing the address of the variable
    printf("the address of var is %x\n", &var);

    // Make use of a function pointer to call another function
    print_functs(5, &print_me);

    // Make effective use of pointer arithmetic to traverse an array
    int nums[] = {1, 2, 3};
    int *ptr = nums;
    for (int i = 0; i < 3; i++) {
        printf("%d\n", *ptr);
        ptr++;
    }

    return 0;
}

void print_me(void)
{
    puts("I'm a function that got called via pointer!");
}

void print_functs(int times, void (*f)(void))
{
    for (int i = 0; i < times; i++) {
        (*f)();
    }
}