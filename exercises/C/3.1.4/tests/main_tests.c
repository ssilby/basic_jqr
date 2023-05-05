#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnitCI.h>
#include "arrays_test.h"



int main(int argc, char* argv[])
{
    

     if (CUE_SUCCESS != CU_initialize_registry())
     {
         return CU_get_error();
     }
     CU_pSuite array_test_suite = CU_add_suite("array_tests", NULL, NULL);
     CU_pSuite matrix_test_suite = CU_add_suite("matrix_tests", NULL, NULL);
     CU_pSuite cumulative_suite = CU_add_suite("cumulative_suite", NULL, NULL);
 
    if ( ( NULL == array_test_suite )  || 
         ( NULL == matrix_test_suite ) || 
         ( NULL == cumulative_suite ) )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
      
    if ( ( NULL == CU_add_test(array_test_suite, "test of create_array()", jqr_test_create_array) ) || 
         ( NULL == CU_add_test(array_test_suite, "test of set_array_node()", jqr_test_set_array_node) ) || 
         ( NULL == CU_add_test(array_test_suite, "test of print_array()", jqr_test_print_array) ) ||
         ( NULL == CU_add_test(array_test_suite, "test of free_array()", jqr_test_free_array) ) ||
         ( NULL == CU_add_test(matrix_test_suite, "test of create_matrix()", jqr_test_create_matrix) ) ||
         ( NULL == CU_add_test(matrix_test_suite, "test of free_matrix()", jqr_test_free_matrix) ) ||
        ( NULL == CU_add_test(cumulative_suite, "JQR 3.1.4", jqr_test_arrays) ) 
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

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}