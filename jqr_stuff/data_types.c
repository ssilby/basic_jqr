#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
    //4.1.3
    
    // char
    char *char_variable = "string";
    printf("this is a: %s\n", char_variable);

    //short
    short short_int = 5;
    printf("this is a short: %hd\n", short_int);

    //int
    int int_int = 5000000;
    printf("this is an int: %d\n", int_int);

    //long
    long big_int = 2147483647;
    printf("this is a long: %ld\n", big_int);

    //long long
    long long bigger_int = 18446744073709551; 
    printf("this is a long 615long: %lld\n", bigger_int);

    //float
    float floaty_boy = 2.435;
    printf("this is a float: %f\n", floaty_boy);

    //double
    double safer_floaty_boy = 1456432.63;
    printf("this is a double: %lf\n", safer_floaty_boy);

    //long double
    long double big_floaty_boy = 1324564654894143.3655;
    printf("this is a long double: %Lf\n", big_floaty_boy);

    // 4.1.4

    int8_t int8_var = -1;
    printf("int8_t: %d\n", int8_var);
    uint8_t uint8_var = 1;
    printf("uint8_t: %u\n", uint8_var);
    int16_t int16_var = -2;
    printf("int16_t: %d\n", int16_var);
    uint16_t uint16_var = 2;
    printf("uint16_t: %u\n", uint16_var);
    int32_t int32_var = -3;
    printf("int32_t: %d\n", int32_var);
    uint32_t uin32_var = 3;
    printf("uint32_t: %u\n", uin32_var);
    int64_t int64_var = -4;
    printf("int64_t: %ld\n", int64_var);
    uint64_t uint64_var = 4;
    printf("uint64_t: %lu\n", uint64_var);


    //4.1.5

    char *array_example = "This is an array\n";
    printf("This is a string array: %s", array_example);

    puts("multi-dimensional\n");
    int multi_example[3][3] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("row %d: %d\n", i, multi_example[i][j]);
        }
    }


    //4.1.6
    int i = 2;
    puts("Addition");
    printf("2 + 2 = %d\n", 2+2);
    puts("Subtraction");
    printf("2 - 2 = %d\n", 2-2);
    puts("Multiplication");
    printf("2 * 2 = %d\n", 2*2);
    puts("Division");
    printf("2 / 2 = %d\n", 2/2);
    puts("Modulus");
    printf("2 modulus 2 = %d\n", 2%2);
    puts("Pre-Increment (++i)");
    ++i;
    printf("++2 = %d\n", i);
    puts("Post-Increment (i++)");
    i++;
    printf("3++ = %d\n", i);
    puts("Pre-Decrement (--i)");
    --i;
    printf("--4 = %d\n", i);
    puts("Post-Decrement (i--)");
    i--;
    printf("3-- = %d\n", i);


    
    return 0;
}