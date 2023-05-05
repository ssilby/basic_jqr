#ifndef JQR_38_PARTIAL_WRITE_TESTS_C
#define JQR_38_PARTIAL_WRITE_TESTS_C

#include "partial_write_tests.h"


char * create_random_fifo()
{
    uint64_t random_uint64 = 0;
    time_t t;
    srand((unsigned) time(&t));
    uint8_t index = 0;
    uint16_t filename_length = 16; // "/tmp/" + 10 char fn + null
    char * new_fifo_fn = malloc(filename_length);

    uint8_t new_fifo_return_val = 0;

    if (NULL != new_fifo_fn)
    {
        memcpy(new_fifo_fn, "/tmp/",5);

        for (index=0; index<10; index++)
        {
            new_fifo_fn[5+index] = ( rand() % 78 ) +48; //ascii range 48-127
        }
        new_fifo_fn[filename_length-1] = 0; 
        new_fifo_return_val = mkfifo(new_fifo_fn, 0660); 
        if (0 != new_fifo_return_val) 
        {
            free(new_fifo_fn);
            new_fifo_fn = NULL;
        }        
    }

    return new_fifo_fn;
}

void * conduct_threaded_write_test(void *in_args)
{
    pthread_test_args *args = (pthread_test_args *) in_args; //sometimes C is stupid
    char *fifo_fn = args->fifo_fn;
    char *data = args->data;
    uint64_t data_len = args->data_len;
    int test_fd = open(fifo_fn,O_WRONLY);
    if(-1 == test_fd)
    {
        err("error opening fifo for writing");
    }
    uint64_t offset = 0;
    uint64_t chunk = 0;
    uint8_t delay_sec = 0;
    uint32_t delay_nano_sec;
    uint64_t current_sent = 0;
    time_t t;
    srand((unsigned) time(&t));
    uint8_t index = 0;
    uint64_t bytes_sent = 0;
    struct timespec req = { 0, 0};
    struct timespec rem = {0, 0};
    while ( data_len > bytes_sent )
    {
        delay_sec = 0;
        delay_nano_sec = rand() % 999999999;
        chunk = rand() % 6 + 1;
        if (data_len < ( bytes_sent + chunk) ) 
        {
            chunk = data_len - bytes_sent;
        }
        req.tv_nsec = delay_nano_sec;
        req.tv_sec = delay_sec;
        nanosleep(&req, &rem); 
        current_sent = write(test_fd, data+offset,chunk);
        if ( -1 == current_sent )
        {
            break;
        }
        bytes_sent += current_sent;
        offset += chunk;
    }
    close(test_fd);
}


void test_partial_read_deserialize_nested_dict()
{
    print_test_name();
    char  * fifo_fn = create_random_fifo();

    if (NULL == fifo_fn)
    {
        err("creating fifo");
    }
    pthread_t write_thread;
    pthread_test_args test_data;
    proto_item_node_t *new_object = NULL;
    uint8_t result = 0;
    int test_fd = -1;

    char serialized[111] =   "\x04"
                            "\x00\x01\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x99\xAA\xBB\xCC\xDD\xEE\xFF\x00"
                            "\x00\x02\x01"
                                 "\x00\x06\x01test.\x00"
                                 "\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x01\x02"
                                "\x00\x00\x00\x00\x00\x00\x00\x7B"
                                "\x00\x06\x01test.\x00"
                            "\x01\x01\x03"
                                "\x00\x00\x00\x00\x00\x00\x01\x41"
                                    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                    "\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                                    "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01"
                                    "\x01\x01\x00\x00\x00\x00\xFF\xFF\xFF\xFF";

    proto_item_node_t sample_data_array_nodes[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_INT64, .value.int64 = -1 },
        { .type = PROTO_INT64, .value.int64 = 1 },
        { .type = PROTO_INT64, .value.int64 = 0xFFFFFFFF }
    };

    proto_array_t sample_data_array_array = {
        .size = 4,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes
    };

    proto_string_t sample_data_str_string = {
        .length = 6,
        .string = "test."

    };

    proto_dict_item_t sample_data_dict_nodes[] = {
        {
            .value = { .type = PROTO_UINT64, .value.uint64 = 0x99AABBCCDDEEFF00 },
            .key = { .type = PROTO_INT64, .value.int64 = 0 }
        },
        {
            .value = { .type = PROTO_INT64, .value.int64 = 0 },
            .key = { .type = PROTO_STRING, .value.string_p = &sample_data_str_string }
        },
        {
            .value = { .type = PROTO_STRING, .value.string_p = &sample_data_str_string },
            .key = { .type = PROTO_INT64, .value.int64 = 123 }
        },
        {
            .value = { .type = PROTO_ARRAY, .value.array_p = &sample_data_array_array },
            .key = { .type = PROTO_INT64, .value.int64 = 321 }
        }
    };

    proto_dict_t sample_data_dict_array= {
        .size = 4,
        .items = (proto_dict_item_t *) &sample_data_dict_nodes
    };

    proto_item_node_t sample_data_dict = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_array
    };

    test_data.fifo_fn = fifo_fn;
    test_data.data = (char *) &serialized;
    test_data.data_len = 111;

    pthread_create( &write_thread, NULL, &conduct_threaded_write_test, (void*) &test_data);

    test_fd = open(fifo_fn,O_RDONLY);

    new_object = proto_deserialize(test_fd);

    pthread_join( write_thread, NULL);

    CU_ASSERT_PTR_NOT_NULL(new_object);
    result = equal_objects(new_object,&sample_data_dict);
    CU_ASSERT_EQUAL(result,1);


    if ( 1 != result )
    {
        printf("\n\tYour Code Created:\n");
        print_proto_item(new_object,1);
        printf("\tTest Cases Expected:\n");
        print_proto_item(&sample_data_dict,1);
    }
    cleanup_proto_item_node_t(new_object);

    close(test_fd);

    if (NULL != fifo_fn)
    {
        unlink(fifo_fn);
    }


    free(fifo_fn);
}


void * conduct_threaded_read_test(void * in_args)
{
    pthread_test_args *args = (pthread_test_args *) in_args;
    char *fifo_fn = args->fifo_fn;
    char *data = args->data;
    uint64_t data_len = args->data_len;
    uint8_t max_chunk_size = 7;
    char chunk_buff[max_chunk_size];
    memset(&chunk_buff,0,max_chunk_size);
    int test_fd = open(fifo_fn,O_RDONLY);
    if(-1 == test_fd)
    {
        err("error opening fifo for reading");
    }
    uint64_t offset = 0;
    uint64_t chunk_len = 0;
    uint8_t delay_sec = 0;
    uint32_t delay_nano_sec;
    uint64_t current_read = 0;
    time_t t;
    srand((unsigned) time(&t));
    uint8_t index = 0;
    uint64_t bytes_read = 0;
    struct timespec req = { 0, 0};
    struct timespec rem = {0, 0};
    while ( 1 )
    {
        delay_sec = 0;
        delay_nano_sec = rand() % 999999;
        chunk_len = max_chunk_size;
        req.tv_nsec = delay_nano_sec;
        req.tv_sec = delay_sec;
        nanosleep(&req, &rem); 
        current_read = read(test_fd, chunk_buff,chunk_len);

        if ( -1 == current_read )
        {
            err("could not read from fd");
            break;
        }
        if ( 0 == current_read ) // EOF
        {
            break;
        }
        bytes_read += current_read;
        if (bytes_read > data_len )
        {
            data = realloc(data,data_len * 2);
            data_len *=2;
        }
        memcpy(data+offset,&chunk_buff,current_read);
        offset += current_read;
    }
    args->data_len = bytes_read;
    args->data = data;
    close(test_fd);
}


void test_partial_write_serialize_nested_dict()
{
    print_test_name();
    char  * fifo_fn = create_random_fifo();

    if (NULL == fifo_fn)
    {
        err("creating fifo");
    }

    time_t t;
    uint64_t index;
    int8_t last_window = 0;
    uint32_t window_offset = 0;
    uint16_t window_width = 0;
    int64_t random_int64 = 0;
    srand((unsigned) time(&t));
    uint32_t long_string_length = 0x3FFFF + ( (rand() % 0x10000) * (rand() % 4) );
    uint32_t string_serialized_length = 1 + long_string_length + ( ( long_string_length / 0xFFFF ) * 3 );
    if ( ( long_string_length % 0xFFFF ) > 0 )
    {
        string_serialized_length += 3;
    }
    char long_string[long_string_length];
    
    for (index = 0; index < long_string_length-1; index++) 
    {
        long_string[index] = 33 + (rand() % 93); // ascii range 33-126
    }
    long_string[long_string_length-1] = 0x00;

    char string_serialized[string_serialized_length];
    string_serialized[0] = PROTO_STRING;

    for (index = 0; index < long_string_length; index += 0xFFFF)
    {
        window_width = 0xFFFF;
        if (index + 0xFFFF >= long_string_length)
        {
            last_window = 1;
            window_width = long_string_length - index;
        }     
        string_serialized[index+window_offset+3] = last_window;
        memcpy(string_serialized+index+4+window_offset,long_string+index,window_width);
        window_width = htobe16(window_width);
        memcpy(string_serialized+index+1+window_offset,&window_width,2);
        window_offset += 3;
    }

    proto_string_t sample_empty_string = {
        .length = long_string_length,
        .string = long_string // note, this string length means it is not null-terminated
    };


    pthread_t read_thread;
    pthread_test_args test_data;
    proto_item_node_t *new_object = NULL;
    uint8_t result = 0;
    int test_fd = -1;
    uint64_t non_string_length = 112;
    uint64_t buffer_size = non_string_length + string_serialized_length;
    char *captured_data = calloc(1, buffer_size);
    char serialized[buffer_size];
    memcpy(&serialized,     "\x04"
                            "\x00\x01\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x99\xAA\xBB\xCC\xDD\xEE\xFF\x00"
                            "\x00\x02\x01"
                                 "\x00\x06\x01test.\x00"
                                 "\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x01\x02"
                                "\x00\x00\x00\x00\x00\x00\x00\x7B"
                                "\x00\x06\x01test.\x00"
                            "\x01\x01\x03"
                                "\x00\x00\x00\x00\x00\x00\x01\x41"
                                    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                    "\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                                    "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01"
                                    "\x00\x01\x00\x00\x00\x00\xFF\xFF\xFF\xFF"
                                    "\x01",
                                    non_string_length);
    
    memcpy( ( (char *) &serialized) + non_string_length,string_serialized,string_serialized_length);
    // we are adding these together to make string longer than page_size 
    proto_item_node_t sample_data_array_nodes[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_INT64, .value.int64 = -1 },
        { .type = PROTO_INT64, .value.int64 = 1 },
        { .type = PROTO_INT64, .value.int64 = 0xFFFFFFFF },
        { .type = PROTO_STRING, .value.string_p = &sample_empty_string }
    };

    proto_array_t sample_data_array_array = {
        .size = 5,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes
    };

    proto_string_t sample_data_str_string = {
        .length = 6,
        .string = "test."

    };

    proto_dict_item_t sample_data_dict_nodes[] = {
        {
            .value = { .type = PROTO_UINT64, .value.uint64 = 0x99AABBCCDDEEFF00 },
            .key = { .type = PROTO_INT64, .value.int64 = 0 }
        },
        {
            .value = { .type = PROTO_INT64, .value.int64 = 0 },
            .key = { .type = PROTO_STRING, .value.string_p = &sample_data_str_string }
        },
        {
            .value = { .type = PROTO_STRING, .value.string_p = &sample_data_str_string },
            .key = { .type = PROTO_INT64, .value.int64 = 123 }
        },
        {
            .value = { .type = PROTO_ARRAY, .value.array_p = &sample_data_array_array },
            .key = { .type = PROTO_INT64, .value.int64 = 321 }
        }
    };

    proto_dict_t sample_data_dict_array= {
        .size = 4,
        .items = (proto_dict_item_t *) &sample_data_dict_nodes
    };

    proto_item_node_t sample_data_dict = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_array
    };

    test_data.fifo_fn = fifo_fn;
    test_data.data = captured_data;
    test_data.data_len = buffer_size;

    pthread_create( &read_thread, NULL, &conduct_threaded_read_test, (void*) &test_data);

    test_fd = open(fifo_fn,O_WRONLY);

    int ret = fcntl(test_fd, F_SETPIPE_SZ, 25); // will round to page size
    if (ret < 0) {
        perror("set pipe size failed.");
    }


    result = proto_serialize(test_fd,&sample_data_dict);

    close(test_fd);

    CU_ASSERT_EQUAL(result,0);

    pthread_join( read_thread, NULL);

    if (test_data.data_len != buffer_size)
    {
        printf("\n\tExpected Length:\t%ld\n\tActual Length:\t\t%ld",buffer_size, test_data.data_len);
    }
    CU_ASSERT_EQUAL(test_data.data_len,buffer_size);
    result = equal_memory(test_data.data,serialized,buffer_size);
    CU_ASSERT_EQUAL(result,1);

    if (result != 1)
    {
        printf("\n\tExpected:\t");
        print_buffer_as_hex(serialized,buffer_size);
        printf("\tActual:\t\t");
        print_buffer_as_hex(test_data.data,test_data.data_len);

    }


    free(test_data.data);

    if (NULL != fifo_fn)
    {
        unlink(fifo_fn);
    }


    free(fifo_fn);
}





void partial_read_test(void)
{
    printf("\n");
    test_partial_read_deserialize_nested_dict();

    printf("\tTest Result: ");

}


void partial_write_test(void)
{
    printf("\n");
    test_partial_write_serialize_nested_dict();

    printf("\tTest Result: ");

}









#endif 