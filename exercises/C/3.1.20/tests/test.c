#include <stdlib.h>
#include <stdint.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "test.h"
#include "../include/bitwise_operators.h"

uint32_t test_3_1_20_a() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a & b;
        uint32_t res = question_a(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

uint32_t test_3_1_20_b() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a | b;
        uint32_t res = question_b(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

uint32_t test_3_1_20_c() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() % 100;
        uint32_t c = a ^ b;
        uint32_t res = question_c(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

uint32_t test_3_1_20_d() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = ~a;
        uint32_t res = question_d(a);
        if(res == b)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

uint32_t test_3_1_20_e() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = a << 1;
        uint32_t res = question_e(a);
        if(res == b)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

uint32_t test_3_1_20_f() {
    uint32_t score = 0;
    uint32_t i;
    for(i = 0; i < ROUNDS_3_1_20; i++) {
        uint32_t a = rand() % 100;
        uint32_t b = rand() & 5;
        uint32_t c = a >> b;
        uint32_t res = question_f(a, b);
        if(res == c)
            score++;
    }
    if(score==ROUNDS_3_1_20)
        return 1;
    return 0;
}

void test_3_1_20() {
    CU_ASSERT_EQUAL(test_3_1_20_a(), 1);
    CU_ASSERT_EQUAL(test_3_1_20_b(), 1);
    CU_ASSERT_EQUAL(test_3_1_20_c(), 1);
    CU_ASSERT_EQUAL(test_3_1_20_d(), 1);
    CU_ASSERT_EQUAL(test_3_1_20_e(), 1);
    CU_ASSERT_EQUAL(test_3_1_20_f(), 1);
}
