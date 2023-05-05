#ifndef JQR_38_HELPER_FUNCTIONS_H
#define JQR_38_HELPER_FUNCTIONS_H
#include "../include/serialize.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>   
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include <time.h>    // time()
#include <errno.h>
#include <CUnit/CUnitCI.h>
#include <CUnit/CUnit.h>

#define  print_test_name() \
    do { fprintf(stdout, "\t-> Running Test: %s()\n", __func__); } while (0)


/**
 * @brief prints the contents of a buffer as hex
 * 
 * @param buffer pointer to buffer
 * @param buffer_length number of bytes to print
 */
void print_buffer_as_hex(char * buffer, uint64_t buffer_length);

/**
 * @brief uses the student function to deserialize the data in the buffer and compares it to the expected object to ensure it is correct
 * 
 * @param data_in - a pointer to the buffer of data to send to the student deserialize function 
 * @param data_in_length - the length of the data_in buffer
 * @param expected_object - a pointer to the object to compare against the student object to check for correctness
 */
void deserialize_and_check_object(char * data_in, uint64_t data_in_length, proto_item_node_t * expected_object);

/**
 * @brief uses the student function to serialize the given object and compares it to expected data
 * 
 * @param object - a pointer to the object that should be serialized
 * @param expected_data - a pointer to the buffer containing the expected value when the object is serialized correctly
 * @param expected_data_length - the length of the expected data
 * @param show_data - if value is 1, will display the expected and received data as hex in a debug message
 */
void serialize_and_check_object(proto_item_node_t * object, char * expected_data, uint64_t expected_data_length, uint8_t show_data);

/**
 * @brief prints a textual representation of an item.
 * 
 * @param item the object to print
 * @param depth when called recursively, this provides the indentaiton offset to
 * use for nested calls
 */
void print_proto_item(proto_item_node_t * item, uint32_t depth);


uint8_t equal_memory(char * first, char * second, uint64_t length);
uint8_t equal_objects(proto_item_node_t * first, proto_item_node_t * second);

uint64_t get_contents_from_fd(int fd, char ** dest);
int8_t write_data_to_fd(int fd, const void * data_buffer, uint64_t data_buffer_length);

void close_test_fd(int fd);
int create_test_fd();

void cleanup_proto_item_node_t(proto_item_node_t * obj);
void free_proto_string_t(proto_string_t *string_p);
void free_proto_item_node_array(proto_item_node_t *array, uint64_t size);
void free_proto_array_t(proto_array_t *array_p);
void free_proto_dict_array(proto_dict_item_t *array, uint64_t size);
void free_proto_dict_t(proto_dict_t *dict_p);
void free_proto_item_node_objects(proto_item_node_t * node);



void err(char * msg);

#endif