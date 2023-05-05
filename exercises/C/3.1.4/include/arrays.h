#ifndef JQR_314_ARRAYS_H
#define JQR_314_ARRAYS_H

#include <stdint.h>
#include <float.h>
#include <stdio.h>
#include <unistd.h>

/*
 
   JQR Items Covered: 

       3.1.4:
      title: 'Demonstrate the ability to create and implement a function that uses
        different arrays:'
      condition: C
      standard: B
      sub_items:
        3.1.4.a: an array
        3.1.4.b: a multi-dimensional array
 
 */

/**
 * @brief whenever writing to stdout, functions must write to this file descriptor to allow for test cases to monitor standard out.
 *   see: dprintf, vdprintf, write
 * 
 */
extern volatile int g_arrays_fd_stdout;

/**
 * @brief whenever writing to stderr, functions must write to this file descriptor to allow for test cases to monitor standard err.
 *   see: dprintf, vdprintf, write
 * 
 */
extern volatile int g_arrays_fd_stderr;

/**
 * @brief whenever reading from stdin, functions must read from this file descriptor to allow for test cases to provide input to stdin
 *   see: dprintf, vdprintf, write
 * 
 */
extern volatile int g_arrays_fd_stdin;

#define NODE_TYPE_UINT16    0
#define NODE_TYPE_UINT32    1
#define NODE_TYPE_UINT64    2
#define NODE_TYPE_INT16     3
#define NODE_TYPE_INT32     4
#define NODE_TYPE_INT64     5
#define NODE_TYPE_FLOAT     6
#define NODE_TYPE_DOUBLE    7
#define NODE_TYPE_C         8
#define NODE_TYPE_C_PTR     9
#define NODE_TYPE_VOID_PTR  10

/**
 * @brief Array Node Entry is a union of various possible elements in the array. Includes signed and unsigned standard integers, floats, char and char pointers, and a void pointer.
 * 
 */

typedef union _array_node_entry
{
	uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
    int16_t int16;
    int32_t int32;
    int64_t int64;
	float f;
	double d;
	char* p_c; // Must be a null-terminated string, and is stored in the heap
	char c;
	void* p_v;
} array_node_entry_t;


/**
 * @brief Array Node is a struct consisting of a union, `entry`, and the type
 * 
 */
typedef struct _array_node
{
    array_node_entry_t entry;
    uint8_t type;
} array_node_t;

/**
 * @brief Array is a struct consisting of an array of array nodes and the number of arrays
 * 
 */
typedef struct _array 
{
    array_node_t* p_array;
    uint64_t len;
} array_t;


/**
 * @brief Matrix is a struct consisting of an array of arrays of length len
 * 
 */
typedef struct _matrix 
{
    array_t** pp_array;
    uint64_t len;
} matrix_t;


/**
 * @brief Create an array of array node objects of size n
 * 
 * @param n unsigned 64 bit integer to define the number of nodes in the array
 * @return array_node* returns a pointer to the array of array nodes  or NULL on error, or when n is 0
 */

array_t* create_array(uint64_t n);


/**
 * @brief Create a matrix object, which is an array of length x containing an array of array nodes of length y
 * 
 * @param x determines the number of arrays to create in the matrix
 * @param y determine the number of nodes in each array 
 * @return matrix_t* a pointer to the matrix object or NULL on error; returns NULL if x or y is 0
 */

matrix_t * create_matrix(uint64_t x, uint64_t y);

/**
 * @brief Set the value of a node at the offset in an array to the given type and value
 * 
 * @param p_array a pointer to the array to be modified
 * @param offset the offset within the array
 * @param type the type of the node, based on the NODE_TYPE_* types defined in the Arrays.h file
 * @param val the value of the object to set. WARNING: this value is limited to a 64bits and is casted to the appropriate type
 * @return int8_t 0 on success and -1 on error
 */


int8_t set_array_node(array_t * p_array, uint64_t offset, uint8_t type, uint64_t val);




/**
 * @brief Prints the array as a line, where each entry is separated by a commas. writes to g_arrays_stdout;
 * each entry should be printed using the correct format-string character according to its type as listed 
 * in the various NODE_TYPE_* definitions. Pointers may be printed as pointers. Individual characters
 * should be printed as such, even if they aren't in the visible ASCII range.
 * 
 * @param p_array pointer to the array to be printed
 */
void print_array(array_t * p_array);


/**
 * @brief Prints the matrix in a csv format. Each line contains a single array separated by commas. 
 * 
 * 
 * @param p_matrix pointer to the matrix to be printed
 */
void print_matrix(matrix_t * p_matrix);

/**
 * @brief Frees all items in the array struct, including the struct, its array, and any associated children
 * 
 * @param p_array pointer to the object to be freed
 */

void free_array(array_t * p_array);

/**
 * @brief Frees all items in the matrix structin, including the struct, its array, and any associated children
 * 
 * @param p_matrix pointer to the object to be freed
 */
void free_matrix(matrix_t * p_matrix);



#endif
