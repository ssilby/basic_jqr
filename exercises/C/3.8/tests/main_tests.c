#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnitCI.h>
#include "general_tests.h"
#include "partial_write_tests.h"
#include "edge_cases.h"
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>   
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*

3.8.1: Demonstrate the ability to handle partial reads and writes during serialization
  and de-serialization

3.8.2: Demonstrate the ability to serialize fixed-size, multi-byte types between systems
  of different endianness

-serialize ints

3.8.3: demonstrate the ability to serialize and de-serialize variable-sized data structures
  between systems of differing endianness

-serialize/deserialize
  - ints
  - strings
  - arrays
  - dictionaries


*/
int main(int argc, char* argv[])
{
    

     if (CUE_SUCCESS != CU_initialize_registry())
     {
         return CU_get_error();
     }
     CU_pSuite general_tests = CU_add_suite("general_tests", NULL, NULL);
     CU_pSuite edge_cases = CU_add_suite("edge_cases", NULL, NULL);
     CU_pSuite partial_writes = CU_add_suite("partial_io", NULL, NULL);
 
    if ( ( NULL == general_tests )  || 
         ( NULL == edge_cases ) ||
         ( NULL == partial_writes ) )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
      
    if ( ( NULL == CU_add_test(general_tests, "test of serialize_uint64()", serialize_uint64) ) || 
         ( NULL == CU_add_test(general_tests, "test of serialize_int64()", serialize_int64) ) || 
         ( NULL == CU_add_test(general_tests, "test of deserialize_uint64()", deserialize_uint64) ) ||
         ( NULL == CU_add_test(general_tests, "test of deserialize_int64()", deserialize_int64) ) ||
         ( NULL == CU_add_test(general_tests, "test of deserialize_string()", deserialize_string) ) ||
         ( NULL == CU_add_test(general_tests, "test of serialize_string()", serialize_string) ) ||
         ( NULL == CU_add_test(general_tests, "test of serialize_array()", serialize_array) ) ||
         ( NULL == CU_add_test(general_tests, "test of deserialize_array()", deserialize_array) ) ||
         ( NULL == CU_add_test(general_tests, "test of serialize_dict()", serialize_dict) ) ||
         ( NULL == CU_add_test(general_tests, "test of deserialize_dict()", deserialize_dict) ) ||
         ( NULL == CU_add_test(edge_cases, "test of check_null_outer_objects()", check_null_outer_objects) ) ||
         ( NULL == CU_add_test(edge_cases, "test of check_null_inner_objects()", check_null_inner_objects) ) ||
         ( NULL == CU_add_test(edge_cases, "test of check_invalid_formats()", check_invalid_formats) ) ||
         ( NULL == CU_add_test(edge_cases, "test of fuzz_deserialize()", fuzz_deserialize) ) ||
         ( NULL == CU_add_test(partial_writes, "test of partial_read_test()", partial_read_test) ) ||
         ( NULL == CU_add_test(partial_writes, "test of partial_write_test()", partial_write_test) ) ||
         ( 0 == 1 ) 
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