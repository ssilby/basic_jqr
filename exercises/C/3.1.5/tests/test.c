#include <stdlib.h>
#include <stdint.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "test.h"
#include "../include/operators.h"

uint32_t test_3_1_5_a() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a + b;
        uint32_t res = question_a(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_b() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a - b;
        uint32_t res = question_b(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_c() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a * b;
        uint32_t res = question_c(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_d() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = (rand() % 10) + 1; //make sure its not 0
        uint32_t c = a / b;
        uint32_t res = question_d(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_e() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = (rand() % 10) + 1; //make sure its not 0
        uint32_t c = a % b;
        uint32_t res = question_e(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_f() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = a + 1;
        uint32_t res = question_f(a);
        if(res == b)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

uint32_t test_3_1_5_g() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_5; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = a - 1;
        uint32_t res = question_g(a);
        if(res == b)
            score++;
    }
    if(score==ROUNDS_3_1_5)
        return 1;
    return 0;
}

void test_3_1_5() {
    CU_ASSERT_EQUAL(test_3_1_5_a(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_b(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_c(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_d(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_e(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_f(), 1);
    CU_ASSERT_EQUAL(test_3_1_5_g(), 1);
}
