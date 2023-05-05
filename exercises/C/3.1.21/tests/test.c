#include <stdlib.h>
#include <stdint.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "test.h"
#include "../include/preprocessor.h"

u_int32_t test_question_a() {

    return question_a()?1:0;
}

u_int32_t test_question_b() {

    return question_b()?1:0;
}

u_int32_t test_question_c() {

    return question_c()?1:0;
}

u_int32_t test_question_d() {

    return question_d()?1:0;
}

u_int32_t test_question_e() {

    return question_e()?1:0;
}

u_int32_t test_question_f() {

    return question_f()?1:0;
}

u_int32_t test_question_g() {

    return question_g()?1:0;
}

void test_3_1_21() {
    CU_ASSERT_EQUAL(test_question_a(), 1);
    CU_ASSERT_EQUAL(test_question_b(), 1);
    CU_ASSERT_EQUAL(test_question_c(), 1);
    CU_ASSERT_EQUAL(test_question_d(), 1);
    CU_ASSERT_EQUAL(test_question_e(), 1);
    CU_ASSERT_EQUAL(test_question_f(), 1);
    CU_ASSERT_EQUAL(test_question_g(), 1);
}
