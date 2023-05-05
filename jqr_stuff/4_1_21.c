#include <stdio.h>

int
main (void)
{
    #ifdef PRE
        printf("you did the preprocessor thingy!\n");
    #endif
        printf ("gooooo\n");
        return 0;
}
