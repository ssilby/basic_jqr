#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnitCI.h>
#include "secure_test.h"

int main(int argc, char *argv[])
{

    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
    CU_pSuite secure_code_test_suite = CU_add_suite("secure_code_tests", NULL, NULL);

    if (NULL == secure_code_test_suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(secure_code_test_suite, "test of name_is_valid()", jqr_test_name_valid)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test of sanitize_name()", jqr_test_sanitize_name)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test bss overflow", jqr_test_bss_overflow)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test main format string", jqr_test_main_format)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test echo", jqr_test_echo)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test play game", jqr_test_play_game)) ||
        (NULL == CU_add_test(secure_code_test_suite, "test zero memory", jqr_test_zero_memory))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);

    if (argc >= 1)
    {
        CU_set_output_filename(argv[0]);
    }

    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();
    CU_list_tests_to_file();

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
