#include "../include/preprocessor.h"



// Add preprocessor directives at the top of this file so that question_a()
// returns true and compiles without errors or warnings
bool question_a() {

    return (sqrt(9) == 3);
}


// Add preprocessor directives at the top of this file so that question_b()
// returns true and compiles without errors or warnings
bool question_b() {

    return (printf("question_b\n") == 11);
}


// Add preprocessor directives at the top of this file so that question_c()
// returns true and compiles without errors or warnings
bool question_c() {

    return (C_ANSWER == 42);
}


// Add preprocessor directives at the top of this file so that question_d()
// returns true and compiles without errors or warnings
bool question_d() {

    return (RECTANGLE_AREA(2, 4) == 8);
}


// Add preprocessor directives at the top of this file so that question_e()
// returns true and compiles without errors or warnings
bool question_e() {

#ifndef E_ANSWER
    #define E_ANSWER 11
#endif

    return (E_ANSWER == 71);
}


// Add preprocessor directives at the top of this file so that question_f()
// returns true and compiles without errors or warnings
bool question_f() {

    return (concat(tr, ue));

}


// Add preprocessor directives at the top of this file so that question_g()
// returns true and compiles without errors or warnings
bool question_g() {

    bool answer = false;

    #ifndef G_ANSWER
        answer = true;
    #endif

    return (answer);

}
