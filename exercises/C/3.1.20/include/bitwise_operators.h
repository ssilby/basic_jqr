#ifndef JQR_3_1_20_BITWISE_OPERATORS_H
#define JQR_3_1_20_BITWISE_OPERATORS_H

#include <stdint.h>


/*
   JQR Items Covered: 

       3.1.20:
      title: 'Demonstrate the ability to create and implement a functions 
         that use bitwise operators:
         
         sub_items:
            3.1.20.a bitwise AND (&)
            3.1.20.b bitwise OR  (|)
            3.1.20.c bitwise XOR (^)
            3.1.20.d bitwise complement (~)
            3.1.20.e SHIFT LEFT  (<<)
            3.1.20.f SHIFT RIGHT (>>)
      
 */



/**
 * @brief Complete the function below to AND
 *   the arguments and return the result
 *
 */
uint32_t question_a(uint32_t arg_1, uint32_t arg_2);

/**
 * @brief Complete the function below to OR
 *   the arguments and return the result
 *
 */
uint32_t question_b(uint32_t arg_1, uint32_t arg_2);

/**
 * @brief Complete the function below to XOR
 *   the arguments and return the result
 *
 */
uint32_t question_c(uint32_t arg_1, uint32_t arg_2);

/**
 * @brief Complete the function below to bitwise complement
 *   the arguments and return the result
 *
 */
uint32_t question_d(uint32_t arg_1);

/**
 * @brief Complete the function below to SHIFT LEFT
 *   the argument by 1 and return the result
 *
 */
uint32_t question_e(uint32_t arg_1);

/**
 * @brief Complete the function below to SHIFT RIGHT
 *   the first argument by the second and return the result
 *
 */
uint32_t question_f(uint32_t arg_1, uint32_t arg_2);


#endif
