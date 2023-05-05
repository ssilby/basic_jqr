#include <stdio.h>
#include <stdlib.h>

int main()
{
    char path[80];
    const char *my_env_var = getenv("PATH");

    if (my_env_var) {
        printf("Path: %s", my_env_var);
    }

    return 0;
}
