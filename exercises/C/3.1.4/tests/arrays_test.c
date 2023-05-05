#ifndef JQR_314_ARRAY_TEST_H
#define JQR_314_ARRAY_TEST_H

#include "../include/arrays.h"
#include <stdlib.h>
#include <CUnit/CUnitCI.h>
#include <CUnit/CUnit.h>



/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/

#define TEST_STDOUT_CAPTURE_FILE "/tmp/CUnit_JQR_314_ARRAY_TEST_stdout.log"

extern volatile int g_arrays_fd_stdout;
static FILE* fp_stdout_file = NULL;
static int cur_stdout_fd;

static void _test_array_contents(array_t * p_array, uint64_t len)
{
    CU_ASSERT_EQUAL(p_array->len,len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(p_array->p_array);

    for(uint64_t i = 0; i < len; i++) 
    {
        CU_ASSERT_EQUAL(p_array->p_array[i].entry.uint64, p_array->p_array[i].entry.uint64); // validate memory exists
        CU_ASSERT_EQUAL(p_array->p_array[i].type, p_array->p_array[i].type); // validate memory exists
    }
}

void jqr_test_create_array(void)
{
    uint64_t array_len = 10;

    array_t* p_new_array = create_array(array_len);

    CU_ASSERT_PTR_NOT_NULL_FATAL(p_new_array);

    _test_array_contents(p_new_array, array_len);

    free(p_new_array->p_array);
    free(p_new_array);

    p_new_array = create_array(0); /// trigger error condition
    CU_ASSERT_PTR_NULL(p_new_array);

}

void jqr_test_create_matrix(void)
{
    uint64_t matrix_x = 10;
    uint64_t matrix_y = 10;

    matrix_t* p_new_matrix = create_matrix(matrix_x,matrix_y);
    CU_ASSERT_PTR_NOT_NULL(p_new_matrix);

    CU_ASSERT_EQUAL(p_new_matrix->len,matrix_x);

    for(uint64_t i=0; i < matrix_x; i++)
    {
        CU_ASSERT_PTR_NOT_NULL(p_new_matrix->pp_array[i]);
        _test_array_contents(p_new_matrix->pp_array[i],matrix_y);
        free(p_new_matrix->pp_array[i]->p_array);
        free(p_new_matrix->pp_array[i]);
    }
    free(p_new_matrix->pp_array);
    free(p_new_matrix);  


    p_new_matrix = create_matrix(0,matrix_y);
    CU_ASSERT_PTR_NULL(p_new_matrix);

    p_new_matrix = create_matrix(matrix_x,0);
    CU_ASSERT_PTR_NULL(p_new_matrix);
    free(p_new_matrix);
}

/**
 * @brief tests set_array_node() as an individual function
 * 
 */

void jqr_test_set_array_node(void)
{
    array_node_t array_node = { 0, 0};
    array_t array = { &array_node, 1 };
    uint64_t offset = 0;
    uint8_t type = NODE_TYPE_UINT64;
    uint64_t val = 0xFFFFFFFFFFFFFFFF;
    CU_ASSERT_EQUAL(set_array_node(&array, offset, type, val),0);
    
    CU_ASSERT_EQUAL(array.p_array->type, type);
    CU_ASSERT_EQUAL(array.p_array->entry.uint64, val);

    CU_ASSERT_EQUAL(set_array_node(NULL,offset,type,val),-1);
    CU_ASSERT_EQUAL(set_array_node(&array,offset+1,type,val),-1);
    array.p_array = NULL;
    CU_ASSERT_EQUAL(set_array_node(&array, offset, type, val),-1);
}


static void _redirect_stdout()
{
    cur_stdout_fd = g_arrays_fd_stdout;
    fp_stdout_file = fopen(TEST_STDOUT_CAPTURE_FILE, "w+");

    if(NULL != fp_stdout_file)
    {
        g_arrays_fd_stdout = fileno(fp_stdout_file);
    }    
}

static char * _capture_and_restore_stdout()
{
    char * p_output = NULL;
    g_arrays_fd_stdout = cur_stdout_fd;

    if (NULL != fp_stdout_file)
    {
        fseek(fp_stdout_file, 0L, SEEK_END);
        uint64_t file_size = ftell(fp_stdout_file);

        fseek(fp_stdout_file, 0, SEEK_SET);

        p_output = calloc(1,file_size+1);
        fread(p_output,file_size, 1, fp_stdout_file);

        fclose(fp_stdout_file);
    }
    fp_stdout_file = NULL;
    if (NULL != p_output)
    {
#ifdef SHOW_OUTPUT
        printf("\n======Captured Output========\n%s======End Captured Output====\n",p_output);
#endif
    }
    
    return p_output;
}

/**
 * @brief tests print_array() as an individual function
 * 
 */
void jqr_test_print_array(void)
{
    array_node_t array_nodes[] = 
        {
            { { .uint16 = 0xFFFF},                NODE_TYPE_UINT16},
            { { .uint32 = 0xFFFFFFFF} ,           NODE_TYPE_UINT32},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_UINT64},
            { { .int16 = 0x7FFF },                NODE_TYPE_INT16},
            { { .int32 = 0xFFFFFFFF },            NODE_TYPE_INT32},
            { { .int64 = 0x7FFFFFFFFFFFFFFF },    NODE_TYPE_INT64},
            { { .f = -1.25},                      NODE_TYPE_FLOAT},
            { { .d = -123456789.987654321},       NODE_TYPE_DOUBLE},
            { { .c = 0x42},                       NODE_TYPE_C}
        };

    array_t array =   {
        (array_node_t*) &array_nodes,
        9
    };
    array_t array2 =   {
        (array_node_t*) NULL,
        9
    };
    char* p_output = NULL;
    _redirect_stdout();
    print_array(&array);
    p_output = _capture_and_restore_stdout();
    CU_ASSERT_STRING_EQUAL(p_output, "65535,4294967295," 
    "18446744073709551615,32767,-1,9223372036854775807,-1.250000"
    ",-123456789.987654,B\n");
    free(p_output);
    _redirect_stdout();
    print_array(NULL);
    print_array(&array2);
    p_output = _capture_and_restore_stdout();
    CU_ASSERT_STRING_EQUAL(p_output, "");
    free(p_output);
}

/**
 * @brief tests print_matrix() as an individual function
 * 
 */
void jqr_test_print_matrix(void)
{
    array_node_t array0_nodes[] = 
        {
            { { .uint64 = 0 },   NODE_TYPE_UINT16},
            { { .uint64 = 0 },   NODE_TYPE_UINT32},
            { { .uint64 = 0 },   NODE_TYPE_UINT64},
            { { .uint64 = 0 },   NODE_TYPE_INT16},
            { { .uint64 = 0 },   NODE_TYPE_INT32},
            { { .uint64 = 0 },   NODE_TYPE_INT64},
            { { .uint64 = 0 },   NODE_TYPE_FLOAT},
            { { .uint64 = 0 },   NODE_TYPE_DOUBLE},
            { { .uint64 = 0x42 },   NODE_TYPE_C}
        };
    array_node_t array1_nodes[] = 
        {
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_UINT16},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_UINT32},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_UINT64},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_INT16},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_INT32},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_INT64},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_FLOAT},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_DOUBLE},
            { { .uint64 = 0xFFFFFFFFFFFFFFFF },   NODE_TYPE_C}
        };
    array_node_t array2_nodes[] = 
        {
            { { .uint64 = 0x7FFF             },   NODE_TYPE_UINT16},
            { { .uint64 = 0x7FFFFFFF         },   NODE_TYPE_UINT32},
            { { .uint64 = 0x7FFFFFFFFFFFFFFF },   NODE_TYPE_UINT64},
            { { .uint64 = 0x7FFF             },   NODE_TYPE_INT16},
            { { .uint64 = 0x7FFFFFFF         },   NODE_TYPE_INT32},
            { { .uint64 = 0x7FFFFFFFFFFFFFFF },   NODE_TYPE_INT64},
            { { .uint64 = 0x7FFFFFFF         },   NODE_TYPE_FLOAT},
            { { .uint64 = 0x7FFFFFFFFFFFFFFF },   NODE_TYPE_DOUBLE},
            { { .uint64 = 0x20               },   NODE_TYPE_C}
        };
   array_t array0 = 
    {
        (array_node_t*) &array0_nodes,
        9
    };
    array_t array1 = 
    {
        (array_node_t*) &array1_nodes,
        9
    }; 
    array_t array2 = 
    {
        (array_node_t*) &array2_nodes,
        9
    };
    array_t * arrays[3] = {
            (array_t* ) &array0,
            (array_t* ) &array1,
            (array_t* ) &array2
            };
    matrix_t matrix = {
        arrays,
        3
    };
    
    char* p_output = NULL;
    _redirect_stdout();
    print_matrix(&matrix);
    p_output = _capture_and_restore_stdout();
    CU_ASSERT_STRING_EQUAL(p_output, "0,0,0,0,0,0,0.000000,0.000000,B\n"
    "65535,4294967295,18446744073709551615,-1,-1,-1,-nan,-nan,\xFF\n"
    "32767,2147483647,9223372036854775807,32767,2147483647,9223372036854775807,nan,nan, \n");
    free(p_output);
}

/**
 * @brief tests free_array() as an individual function
 * 
 */
void jqr_test_free_array(void)
{
    array_t* p_array = calloc(1,sizeof(array_t));

    p_array->len =1;
    p_array->p_array = calloc(1,sizeof(array_node_t));
    free_array(p_array);
}

/**
 * @brief tests free_matrix() as an individual function
 * 
 */

void jqr_test_free_matrix(void)
{
    matrix_t * p_matrix = calloc(1, sizeof(array_t));
    p_matrix->len = 2;
    p_matrix->pp_array = calloc(3, sizeof(array_t *));
    p_matrix->pp_array[0] = calloc(1,sizeof(array_t));

    p_matrix->pp_array[0]->len =1;
    p_matrix->pp_array[0]->p_array = calloc(1,sizeof(array_node_t));
    p_matrix->pp_array[1] = NULL;
    free_matrix(p_matrix);
    CU_PASS("Survived freeing resources.");
}

/**
 * @brief tests all functions together as interworking components
 * 
 */
void jqr_test_arrays(void)
{
    matrix_t * cur_matrix = create_matrix(8,8);
    for(uint64_t i = 0; i < 8; i++) 
    {
        for(uint64_t j = 0; j < 8; j++) 
        {
            CU_ASSERT_EQUAL(set_array_node(cur_matrix->pp_array[i], j, j+1, i+1),0);
        }
    }
    char* p_output = NULL;
    _redirect_stdout();
    print_matrix(cur_matrix);
    p_output = _capture_and_restore_stdout();
    CU_ASSERT_STRING_EQUAL(p_output, "1,1,1,1,1,1.000000,1.000000,\x01\n"
        "2,2,2,2,2,2.000000,2.000000,\x02\n"
        "3,3,3,3,3,3.000000,3.000000,\x03\n"
        "4,4,4,4,4,4.000000,4.000000,\x04\n"
        "5,5,5,5,5,5.000000,5.000000,\x05\n"
        "6,6,6,6,6,6.000000,6.000000,\x06\n"
        "7,7,7,7,7,7.000000,7.000000,\x07\n"
        "8,8,8,8,8,8.000000,8.000000,\x08\n"
    );
    free(p_output);
    free_matrix(cur_matrix);
    CU_PASS("completed using all the functions together.");
}

#endif