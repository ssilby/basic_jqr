#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnitCI.h>

#include "dijkstra_test.h"

int init(void) { return 0; }
int clean(void) { return 0; }

int main(int argc, char **argv) {
    // Init CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_TestInfo tests[] = {
        {"Test basic algorithm", test_basic},
        {"Test null pointer", test_null},
        {"Test only one node", test_one_node},
        {"Test disconnected graph", test_disconnect},
        CU_TEST_INFO_NULL,
    };

    CU_SuiteInfo suites[] = {
        {"dijkstra algorithm test suite", init, clean, NULL, NULL, tests},
        CU_SUITE_INFO_NULL,
    };

    if (CUE_SUCCESS != CU_register_suites(suites)) {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(1);
    }

    if (argc >= 1) {
        CU_set_output_filename(argv[0]);
    }

    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();
    CU_list_tests_to_file();
    
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
