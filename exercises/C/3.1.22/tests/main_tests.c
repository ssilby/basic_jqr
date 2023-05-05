#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnitCI.h>
#include "game_test.h"

int main(int argc, char* argv[])
{
    

     if (CUE_SUCCESS != CU_initialize_registry())
     {
         return CU_get_error();
     }
     CU_pSuite game_test_suite = CU_add_suite("game_tests", NULL, NULL);
 
    if ( ( NULL == game_test_suite ) )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
      
    if ( ( NULL == CU_add_test(game_test_suite, "test start and quit game", jqr_test_play_quit_game) ) || 
         ( NULL == CU_add_test(game_test_suite, "test full play of game", jqr_test_play_full_game ) ) || 
        ( NULL == CU_add_test(game_test_suite, "test travel to death", jqr_test_travel_until_death ) ) || 
        ( NULL == CU_add_test(game_test_suite, "test flee to death", jqr_test_flee_until_death ) ) || 
        ( NULL == CU_add_test(game_test_suite, "test fight to death", jqr_test_fight_until_death ) ) || 
         ( NULL == CU_add_test(game_test_suite, "test use after free", jqr_test_use_after_free) ) || 
        ( 1== 0)
    )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);

    if ( argc >= 1 )
    {
        CU_set_output_filename(argv[0]);
    }
    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();
    CU_list_tests_to_file();

    CU_pRunSummary summary = CU_get_run_summary();
    printf("##############################################\n");
    printf("#             Test Summary:                  #\n");
    printf("# Suites Run:  %5d,\t Failed: %5d       #\n",summary->nSuitesRun, summary->nSuitesFailed);
    printf("# Tests Run:   %5d,\t Failed: %5d       #\n",summary->nTestsRun, summary->nTestsFailed);
    printf("# Asserts Run: %5d,\t Failed: %5d       #\n",summary->nAsserts, summary->nAssertsFailed);
    printf("##############################################\n");
    if(summary->nFailureRecords > 0)
    {
        CU_basic_show_failures(CU_get_failure_list());
    }
      
    CU_cleanup_registry();
    return CU_get_error();
}
