#include <stdlib.h>
#include <stdint.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../include/data_types.h"

uint8_t test_3_1_3_a() {
    if(question_a() == 123)
        return 1;
    return 0;
}

uint8_t test_3_1_3_b() {
    if(question_b() >= 65536)
        return 1;
    return 0;
}

uint8_t test_3_1_3_c() {
    if(question_c() >= 4294967296)
        return 1;
    return 0;
}

uint8_t test_3_1_3_d() {
    if(question_d() == 200)
        return 1;
    return 0;
}

uint8_t test_3_1_3_e() {
    if(question_e() == 234)
        return 1;
    return 0;
}

uint8_t test_3_1_3_f() {
    if(question_f() == 3.45f)
        return 1;
    return 0;
}

uint8_t test_3_1_3_g() {
    if(question_g() == 'A')
        return 1;
    return 0;
}

uint8_t test_3_1_3_h() {
    if(question_h() == 4.56)
        return 1;
    return 0;
}

uint8_t test_3_1_3_i() {
    if(question_i() == 311244)
        return 1;
    return 0;
}

void test_3_1_3(void) {
    CU_ASSERT_EQUAL(test_3_1_3_a(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_b(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_c(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_d(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_e(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_f(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_g(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_h(), 1);
    CU_ASSERT_EQUAL(test_3_1_3_i(), 1);
}
