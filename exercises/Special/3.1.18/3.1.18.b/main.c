#include <stdio.h>
#include <stdlib.h>
#include "include/algo/algo.h"

int main(int argc, char *argv[])
{
    char buffer[20];

    FILE *codefile = fopen("../random.txt", "r");

    if(codefile == NULL)
    {
        puts("Unable to open random.txt");
	exit(1);
    }

    fgets(buffer, 20, codefile);

    size_t code = DoAlgo(buffer);

    printf("Your code is: %zu\n", code);
}
