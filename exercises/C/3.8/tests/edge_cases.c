#ifndef JQR_38_EDGE_CASES_C
#define JQR_38_EDGE_CASES_C

#include "edge_cases.h"




/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/


void print_fd_and_teardown(int fd)
{

    char *fd_buff = NULL;
    uint64_t fd_buff_length = 0;

    fd_buff_length = get_contents_from_fd(fd,&fd_buff);
    close_test_fd(fd);

    if (0<fd_buff_length)
    {
        printf("\t\tCaptured on test fd (%ld): ",fd_buff_length);
        print_buffer_as_hex(fd_buff,fd_buff_length);
    }
    free(fd_buff);
}



void serialize_null(int fd)
{
    print_test_name();
    uint8_t result = 0;

    result = proto_serialize(fd,NULL);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void serialize_negative_fd()
{
    print_test_name();
    int fd = -1;
    uint8_t result = 0;

    proto_item_node_t sample_data_int64 = {
        .type = PROTO_INT64,
        .value.int64 = 0xFFFFFFFFFFFFFFFF
    };

    result = proto_serialize(fd,&sample_data_int64);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void serialize_null_node_string(int fd)
{
    print_test_name();
    uint8_t result = 0;
    proto_item_node_t input_object = {
        .type = PROTO_STRING,
        .value.string_p = NULL
    };
    result = proto_serialize(fd,&input_object);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void serialize_null_node_array(int fd)
{
    print_test_name();
    uint8_t result = 0;
    proto_item_node_t input_object = {
        .type = PROTO_ARRAY,
        .value.array_p = NULL
    };
    result = proto_serialize(fd,&input_object);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void serialize_null_node_dict(int fd)
{
    print_test_name();
    uint8_t result = 0;
    proto_item_node_t input_object = {
        .type = PROTO_DICT,
        .value.dict_p = NULL
    };
    result = proto_serialize(fd,&input_object);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void check_null_outer_objects(void)
{
    int fd = create_test_fd();
    printf("\n");

    serialize_null(fd);
    serialize_negative_fd();
    serialize_null_node_dict(fd);
    serialize_null_node_string(fd);
    serialize_null_node_array(fd);

    print_fd_and_teardown(fd);

    printf("\tTest Result: ");
}


void serialize_null_parts_dict(int fd)
{
    print_test_name();

    uint8_t result = 0;

    proto_dict_t sample_data_dict_simple_array= {
        .size = 1,
        .items = NULL
    };

    proto_item_node_t sample_data_dict_simple = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_simple_array
    };
    result = proto_serialize(fd,&sample_data_dict_simple);

    CU_ASSERT_NOT_EQUAL(result,0);
}


void serialize_null_parts_string(int fd)
{
    print_test_name();

    uint8_t result = 0;

    proto_string_t sample_empty_string = {
        .length = 2,
        .string = NULL 
    };

    proto_item_node_t small_window_string = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    result = proto_serialize(fd,&small_window_string);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void serialize_null_parts_array(int fd)
{
    print_test_name();
    uint8_t result = 0;

    proto_array_t sample_data_array_array_serialized = {
        .size = 2,
        .nodes = NULL
    };

    proto_item_node_t sample_data_array_serialized = {
        .type = PROTO_ARRAY,
        .value.array_p = &sample_data_array_array_serialized
    };
    result = proto_serialize(fd,&sample_data_array_serialized);

    CU_ASSERT_NOT_EQUAL(result,0);
}


void check_null_inner_objects(void)
{
    int fd = create_test_fd();
    printf("\n");

    serialize_null_parts_dict(fd);
    serialize_null_parts_string(fd);
    serialize_null_parts_array(fd);

    print_fd_and_teardown(fd);

    printf("\tTest Result: ");
}

void check_unsupported_type(uint8_t type, int fd)
{
    
    uint8_t result = 0;

    proto_item_node_t sample_null= {
        .type = type,
        .value.int64 = 0
    };
    result = proto_serialize(fd,&sample_null);
    printf("\t\tType was: 0x%02x\n",type);
    
}

void serialize_unsupported_type(int fd)
{
    time_t t;
    srand((unsigned) time(&t));
    for (uint8_t index=5; index>0;index--)
    {
        check_unsupported_type( (rand() % (0xFF - 0x06) ) + 0x05, fd);
    }   
}


void serialize_null_type(int fd)
{
    print_test_name();
    uint8_t result = 0;

    proto_item_node_t sample_null= {
        .type = PROTO_NULL,
        .value.int64 = 0
    };
    result = proto_serialize(fd,&sample_null);

    CU_ASSERT_NOT_EQUAL(result,0);
}

void check_invalid_formats(void)
{
    int fd = create_test_fd();
    printf("\n");
    

    serialize_null_type(fd);
    serialize_unsupported_type(fd);

    print_fd_and_teardown(fd);

    printf("\tTest Result: ");

}

uint16_t build_fuzz_str(char ** str_p)
{
    time_t t;
    srand((unsigned) time(&t));
    uint16_t fuzz_str_len = rand() % 0xFFF;
    uint16_t index = 0;
    char *local_str_p = malloc(fuzz_str_len);
    if (NULL == local_str_p)
    {
        return 0;
    }
    for (index=0; index<fuzz_str_len; index++)
    {
        local_str_p[index] = rand() % 0xFF;
    }
    *str_p = local_str_p;
    return fuzz_str_len;
}

void fuzz_deserialize_once()
{
    int fd = create_test_fd();

    proto_item_node_t *response = NULL;

    char * fuzz_buff = NULL;
    uint16_t fuzz_buff_length = build_fuzz_str(&fuzz_buff);
    write_data_to_fd(fd,fuzz_buff,fuzz_buff_length);
    free(fuzz_buff);
    lseek(fd,0,SEEK_SET);
    response = proto_deserialize(fd);

    if (NULL != response)
    {
        printf("cleaning valid object\n");
        cleanup_proto_item_node_t(response);
    }


    close_test_fd(fd);
}


void fuzz_deserialize(void)
{

    uint16_t count = 0x07FF;

    while (0 < count)
    {
        fuzz_deserialize_once();
        count--;
    }

    CU_PASS("Fuzzing did not cause crash\n");

}



#endif