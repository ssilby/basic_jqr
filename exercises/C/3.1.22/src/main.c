#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/memoryleak.h"

int main()
{
    memory_leak();
    memory_overread();
    memory_orphan();

    return 0;
}

/**
 * @brief Memory overread is when the number of bytes being read are more
 *  than what they should be.
 *
 * Hint - Print the strings to fully understand what is happening
 *
 * @param
 * @return int of 0, procedure ended
 */
int memory_overread()
{
    char *ptr = malloc(12 * sizeof(char));
    char name[20];

    memset(ptr, 0, 11 * sizeof(char));

    strcpy(name, "This is my name now");
    strcpy(ptr,  "my ptr name");

    memcpy(name, ptr, 20);

    return 0;
}

/**
 * @brief Memory is not being freed after it has been created via malloc call.
 *  Memory should always be initialized, use calloc when possible, or
 *  follow a malloc call with an assignment operation or memset call.
 *
 * @param
 * @return int of 0, procedure ended
 */
int memory_leak()
{
    char *leaked = malloc(20 * sizeof(char));

    strcpy((char*)leaked, "This will be leaked");

    return 0;
}

/**
 * @brief If memoryArea is freed by making a call to free, then as a result
 * the newArea pointer also will become invalid. The memory location to which
 * newArea was pointing cannot be freed, as there is no pointer left pointing
 * to that location.
 *
 * Hint - Print the strings and pointer addresses to fully understand what is happening
 *
 * @param
 * @return int of 0, procedure ended
 */
int memory_orphan()
{
    char *memoryArea = malloc(11 * sizeof(char));
    char *newArea = malloc(11 * sizeof(char));

    strcpy((char*)memoryArea, "MEMORYAREA");
    strcpy((char*)newArea, "ITSNEWAREA");

    memoryArea = newArea;

    return 0;
}
