#ifndef JQR_38_GENERAL_TESTS_H
#define JQR_38_GENERAL_TESTS_H



/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/




void serialize_uint64(void);
void serialize_int64(void);
void deserialize_uint64(void);
void deserialize_int64(void);
void deserialize_string(void);
void serialize_string(void);
void serialize_array(void);
void deserialize_array(void);
void serialize_dict(void);
void deserialize_dict(void);


#endif