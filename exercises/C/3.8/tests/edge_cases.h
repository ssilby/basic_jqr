#ifndef JQR_38_EDGE_CASES_H
#define JQR_38_EDGE_CASES_H
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "helper_functions.h"


/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/




void check_null_outer_objects(void);
void check_null_inner_objects(void);
void check_invalid_formats(void);
void fuzz_deserialize(void);


#endif