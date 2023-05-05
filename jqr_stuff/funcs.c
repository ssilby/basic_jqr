#include <stdio.h>
#include <string.h>

void print_func(void);
void print_value(int);
void print_pointer(char *);
int add_function(int);
void modify_param(int *);
void get_input(char *, size_t);
unsigned long long int factorial(unsigned int);

int main(void)
{
    print_func();
    print_value(4);
    char *name = "JQR boss";
    print_pointer(name);
    int new_value = add_function(2);
    printf("passed 2 and returns %d\n", new_value);
    int i = 5;
    modify_param(&i);
    printf("what used to be 5 at the address of i is now: %d\n", i);
    char buffer[20];
    get_input(buffer, sizeof(buffer));
    //A function pointer
    void (*fpointer)(int) = &print_value;
    puts("calling print_value as a function pointer");
    (*fpointer)(5);
    int j = 10;
    printf("Factorial of %d is %lld\n", j, factorial(j));

    return 0;
}

//Proper declaration for created functions

//A function that does not return a value (i.e., is declared void)
void print_func(void)
{
    puts("This function does not have a return value");
    return;
}

//A function that is passed an argument by value
void print_value(int value)
{
    printf("Value passed to this function: %d\n", value);
    return;
}

//A function that takes a pointer argument
void print_pointer(char *point)
{
    printf("Data at the pointer passed to this function: %s\n", point);
    return;
}

//A function that returns a value using a return statement
//Returns value plus 1
int add_function(int value)
{
    return value + 1;
}

//A function that modifies an output parameter through a pointer
void modify_param(int *value)
{
    *value = *value + 1;
    return;
}

//A function that receives input from a user
void get_input(char *buffer, size_t buff_size)
{
    puts("What is your name?");
    fgets(buffer, buff_size, stdin);
    printf("hey there %s\n", buffer);
    return;
}

//A recursive function
// adapted from https://www.tutorialspoint.com/cprogramming/c_recursion.htm
unsigned long long int factorial(unsigned int i)
{
    if (i <= 1) {
        return 1;
    }
    return i * factorial(i - 1);
}
