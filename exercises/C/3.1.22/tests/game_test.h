#ifndef TEST_3_1_22_TESTS
#define TEST_3_1_22_TESTS
#include <stdlib.h>
#include <unistd.h>
#include <CUnit/CUnitCI.h>
#include <CUnit/CUnit.h>
#include <pthread.h> 
#include <fcntl.h> 
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "../include/game.h"
/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/

#define  print_test_name() \
    do { fprintf(stdout, "\t-> Running Test: %s()\n", __func__); } while (0)


void jqr_test_play_quit_game(void);

void jqr_test_play_full_game(void);

void jqr_test_use_after_free(void);

void jqr_test_travel_until_death(void);
void jqr_test_flee_until_death(void);
void jqr_test_fight_until_death(void);

#endif