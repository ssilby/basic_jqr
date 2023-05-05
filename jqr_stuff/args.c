#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//4.1.7

int main(int argc, char *argv[])
{
    
    if (argc > 1) {
        printf("program name: %s\n", argv[0]);
        for (int i = 1; i < argc; i++) {
            printf("argument %d: %s\n", i, argv[i]);
        }
    } else if (argc == 1) {
        printf("program name: %s\n", argv[0]);
    } else {
        printf("this isn't possible\n");
    }

    int x = argc;

    switch (x) {
        case 1:
            printf("No args\n");
            break;
        case 2:
            printf("one arg\n");
            break;
        case 3:
            printf("2 args\n");
            break;
        default:
            printf("more than 2 args\n");
            goto EXIT_HERE;
            break;
    }

    int j = 0;
    do {
        printf("in a do while loop\n");
        j++;
    } while (j < argc);

    int k = 0;
    while (k < argc) {
        printf("in a while loop\n");
        k++;
    }

    EXIT_HERE:
        printf("safe exit point\n");

    return 0;
}