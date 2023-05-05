#ifndef JQR_38_PARTIAL_WRITE_TESTS_H
#define JQR_38_PARTIAL_WRITE_TESTS_H
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <unistd.h>
#include <pthread.h>

#include "helper_functions.h"


/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/



// taken from: #include </usr/include/linux/fcntl.h> // linux specific header to limit pipe size
#define F_LINUX_SPECIFIC_BASE   1024
#define F_SETPIPE_SZ    (F_LINUX_SPECIFIC_BASE + 7)


typedef struct _pthread_test_args
{
    char * fifo_fn;
    char * data;
    uint64_t data_len;
} pthread_test_args;



void partial_read_test(void);

void partial_write_test(void);

#endif