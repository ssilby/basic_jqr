#ifndef JQR_362_SECURE_H
#define JQR_362_SECURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * This is a struct that we will use for testing your code, we don't want 
 * the buffer overflows to segfault and crash your program...
 */
struct test {
    char buf[0x20];
    unsigned int val;
};

bool name_is_valid(char *name);

char *sanitize_name(char *name);

void *zero_memory(void *ptr, size_t size);

int my_main(void);

#endif
