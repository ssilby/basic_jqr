/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/


/**
 * @brief tests create_array() as an individual function. 
 * 
 */
void jqr_test_create_array(void);

/**
 * @brief tests create_matrix() as an individual function
 * 
 */
void jqr_test_create_matrix(void);

/**
 * @brief tests set_array_node() as an individual function
 * 
 */

void jqr_test_set_array_node(void);

/**
 * @brief tests print_array() as an individual function
 * 
 */
void jqr_test_print_array(void);

/**
 * @brief tests print_matrix() as an individual function
 * 
 */
void jqr_test_print_matrix(void);

/**
 * @brief tests free_array() as an individual function
 * 
 */
void jqr_test_free_array(void);

/**
 * @brief tests free_matrix() as an individual function
 * 
 */

void jqr_test_free_matrix(void); 

/**
 * @brief tests all functions together as interworking components
 * 
 */
void jqr_test_arrays(void);