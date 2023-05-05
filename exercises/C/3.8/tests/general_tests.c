#ifndef JQR_38_GENERAL_TESTS_C
#define JQR_38_GENERAL_TESTS_C

#include "../include/serialize.h"
#include "helper_functions.h"
#include "general_tests.h"




/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/









void serialize_min_uint64()
{
    print_test_name();
    char expected[10] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    proto_item_node_t sample_data_uint64 = {
        .type = PROTO_UINT64,
        .value.uint64 = 0
    };

    serialize_and_check_object( &sample_data_uint64, expected, 9, 1);

}


void serialize_max_uint64()
{
    print_test_name();
    char expected[10] = "\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

    proto_item_node_t sample_data_uint64 = {
        .type = PROTO_UINT64,
        .value.uint64 = 0xFFFFFFFFFFFFFFFF
    };

    serialize_and_check_object( &sample_data_uint64, expected, 9, 1);

}

void serialize_middle_uint64()
{
    print_test_name();
    char expected[10] = "\x00\xDE\xAD\xBE\xEF\x00\x00\x00\x00";

    proto_item_node_t sample_data_uint64 = {
        .type = PROTO_UINT64,
        .value.uint64 = 0xDEADBEEF00000000
    };

    serialize_and_check_object( &sample_data_uint64, expected, 9, 1);

}

void serialize_random_uint64()
{
    print_test_name();
    time_t t;
    uint8_t index = 0;
    uint64_t random_uint64 = 0;
    srand((unsigned) time(&t));

    char expected[10];
    expected[0] = 0;
    for(index = 1; index<=8; index++)
    {
        expected[index] = rand() % 256 ;
    }
    memcpy(&random_uint64, expected+1, sizeof(uint64_t));
    random_uint64 = be64toh(random_uint64);
    

    proto_item_node_t sample_data_uint64 = {
        .type = PROTO_UINT64,
        .value.uint64 = random_uint64
    };

    serialize_and_check_object( &sample_data_uint64, expected, 9, 1);

}

void serialize_min_int64()
{
    print_test_name();
    char expected[10] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00";

    proto_item_node_t sample_data_int64 = {
        .type = PROTO_INT64,
        .value.int64 = 0
    };

    serialize_and_check_object( &sample_data_int64, expected, 9, 1);

}


void serialize_max_int64()
{
    print_test_name();
    char expected[10] = "\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

    proto_item_node_t sample_data_int64 = {
        .type = PROTO_INT64,
        .value.int64 = 0xFFFFFFFFFFFFFFFF
    };

    serialize_and_check_object( &sample_data_int64, expected, 9, 1);

}

void serialize_middle_int64()
{
    print_test_name();
    char expected[10] = "\x01\xDE\xAD\xBE\xEF\x00\x00\x00\x00";

    proto_item_node_t sample_data_int64 = {
        .type = PROTO_INT64,
        .value.int64 = 0xDEADBEEF00000000
    };

    serialize_and_check_object( &sample_data_int64, expected, 9, 1);

}

void serialize_random_int64()
{
    print_test_name();
    time_t t;
    uint8_t index = 0;
    int64_t random_int64 = 0;
    srand((unsigned) time(&t));

    char expected[10];
    expected[0] = 1;
    for(index = 1; index<=8; index++)
    {
        expected[index] = rand() % 256 ;
    }
    memcpy(&random_int64, expected+1, sizeof(uint64_t));
    random_int64 = be64toh(random_int64);
    

    proto_item_node_t sample_data_int64 = {
        .type = PROTO_INT64,
        .value.int64 = random_int64
    };

    serialize_and_check_object( &sample_data_int64, expected, 9, 1);

}

void serialize_int64(void)
{
    printf("\n");
    serialize_min_int64();
    serialize_max_int64();
    serialize_middle_int64();
    serialize_random_int64();

    printf("\tTest Result: ");
}

void serialize_uint64(void)
{
    printf("\n");
    serialize_min_uint64();
    serialize_max_uint64();
    serialize_middle_uint64();
    serialize_random_uint64();


    printf("\tTest Result: ");
}


void deserialize_random_uint64()
{
    print_test_name();
    time_t t;
    uint8_t index = 0;
    uint64_t random_uint64 = 0;
    srand((unsigned) time(&t));

    char serialized[10];
    serialized[0] = 0;
    for(index = 1; index<=8; index++)
    {
        serialized[index] = rand() % 256 ;
    }
    memcpy(&random_uint64, serialized+1, sizeof(uint64_t));
    random_uint64 = be64toh(random_uint64); 

    proto_item_node_t expected = {
        .type = PROTO_UINT64,
        .value.int64 = random_uint64
    };
    deserialize_and_check_object(serialized, 9, &expected);
}



void deserialize_middle_uint64()
{
    print_test_name();
    char serialized[10] = "\x00\xDE\xAD\xBE\xEF\x00\x00\x00\x00";

    proto_item_node_t expected = {
        .type = PROTO_UINT64,
        .value.uint64 = 0xDEADBEEF00000000
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_max_uint64()
{
    print_test_name();
    char serialized[10] = "\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

    proto_item_node_t expected = {
        .type = PROTO_UINT64,
        .value.uint64 = 0xFFFFFFFFFFFFFFFF
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_min_uint64()
{
    print_test_name();
    char serialized[10] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    proto_item_node_t expected = {
        .type = PROTO_UINT64,
        .value.uint64 = 0
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_uint64(void)
{
    printf("\n");
    deserialize_min_uint64();
    deserialize_max_uint64();
    deserialize_middle_uint64();
    deserialize_random_uint64();

    printf("\tTest Result: ");

}

void deserialize_middle_int64()
{
    print_test_name();
    char serialized[10] = "\x01\xDE\xAD\xBE\xEF\x00\x00\x00\x00";

    proto_item_node_t expected = {
        .type = PROTO_INT64,
        .value.int64 = 0xDEADBEEF00000000
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_max_int64()
{
    print_test_name();
    char serialized[10] = "\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

    proto_item_node_t expected = {
        .type = PROTO_INT64,
        .value.int64 = 0xFFFFFFFFFFFFFFFF
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_min_int64()
{
    print_test_name();
    char serialized[10] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00";

    proto_item_node_t expected = {
        .type = PROTO_INT64,
        .value.int64 = 0
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_random_int64()
{
    print_test_name();
    time_t t;
    uint8_t index = 0;
    int64_t random_int64 = 0;
    srand((unsigned) time(&t));

    char serialized[10];
    serialized[0] = 1;
    for(index = 1; index<=8; index++)
    {
        serialized[index] = rand() % 256 ;
    }
    memcpy(&random_int64, serialized+1, sizeof(int64_t));
    random_int64 = be64toh(random_int64); 

    proto_item_node_t expected = {
        .type = PROTO_INT64,
        .value.int64 = random_int64
    };
    deserialize_and_check_object(serialized, 9, &expected);
}

void deserialize_int64(void)
{
    printf("\n");

    deserialize_min_int64();
    deserialize_max_int64();
    deserialize_middle_int64();
    deserialize_random_int64();

    printf("\tTest Result: ");

}


void deserialize_empty_string()
{
    print_test_name();
    char serialized[5] = "\x02\x00\x00\x01";

    proto_string_t sample_empty_string = {
        .length = 0,
        .string = ""
    };

    proto_item_node_t expected = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    deserialize_and_check_object(serialized, 4, &expected);
}

void deserialize_multi_window_randow_string()
{
    print_test_name();
    time_t t;
    uint64_t index;
    int8_t last_window = 0;
    uint32_t window_offset = 0;
    uint16_t window_width = 0;
    int64_t random_int64 = 0;
    srand((unsigned) time(&t));
    uint32_t long_string_length = 0xFFFF + ( (rand() % 0x10000) * (rand() % 4) );
    uint32_t serialized_length = 1 + long_string_length + ( ( long_string_length / 0xFFFF ) * 3 );
    if ( ( long_string_length % 0xFFFF ) > 0 )
    {
        serialized_length += 3;
    }
    char long_string[long_string_length];
    
    for (index = 0; index < long_string_length-1; index++) 
    {
        long_string[index] = 33 + (rand() % 93); // ascii range 33-126
    }
    long_string[long_string_length-1] = 0x00;

    char serialized[serialized_length];
    serialized[0] = PROTO_STRING;

    for (index = 0; index < long_string_length; index += 0xFFFF)
    {
        window_width = 0xFFFF;
        if (index + 0xFFFF >= long_string_length)
        {
            last_window = 1;
            window_width = long_string_length - index;
        }     
        serialized[index+window_offset+3] = last_window;
        memcpy(serialized+index+4+window_offset,long_string+index,window_width);
        window_width = htobe16(window_width);
        memcpy(serialized+index+1+window_offset,&window_width,2);
        window_offset += 3;
    }

    proto_string_t sample_empty_string = {
        .length = long_string_length,
        .string = long_string // note, this string length means it is not null-terminated
    };

    proto_item_node_t expected = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    deserialize_and_check_object(serialized, serialized_length, &expected);
}

void deserialize_max_window_string()
{
    print_test_name();
    uint32_t long_string_length = 0x10000;
    uint32_t serialized_length = long_string_length+4;
    char long_string[long_string_length];
    
    uint64_t index;
    for (index = 0; index < long_string_length-1; index++) 
    {
        long_string[index] = 0x42;
    }
    long_string[long_string_length-1] = 0x00;
    char serialized[serialized_length];
    memcpy(serialized,"\x02\xFF\xFF\x01",4);

    memcpy(serialized+4,long_string,long_string_length);

    proto_string_t sample_empty_string = {
        .length = long_string_length-1,
        .string = long_string // note, this string length means it is not null-terminated
    };

    proto_item_node_t expected = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    deserialize_and_check_object(serialized, serialized_length, &expected);
}

void deserialize_small_window_string()
{
    print_test_name();
    char serialized[26] = "\x02\x00\x16\x01This is a test string.";

    proto_string_t sample_empty_string = {
        .length = 22,
        .string = "This is a test string." // note, this string length means it is not null-terminated
    };

    proto_item_node_t expected = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    deserialize_and_check_object(serialized, 26, &expected);
}


void deserialize_string(void)
{
    printf("\n");

    deserialize_empty_string();
    deserialize_small_window_string();
    deserialize_max_window_string();
    deserialize_multi_window_randow_string();

    printf("\tTest Result: ");

}

void serialize_empty_string()
{
    print_test_name();
    char expected[5] = "\x02\x00\x00\x01";

    proto_string_t sample_empty_string = {
        .length = 0,
        .string = ""
    };

    proto_item_node_t proto_empty_string = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    serialize_and_check_object( &proto_empty_string, expected, 4, 1);
}

void serialize_multi_window_randow_string()
{
    print_test_name();
    time_t t;
    uint64_t index;
    int8_t last_window = 0;
    uint32_t window_offset = 0;
    uint16_t window_width = 0;
    int64_t random_int64 = 0;
    srand((unsigned) time(&t));
    uint32_t long_string_length = 0xFFFF + ( (rand() % 0x10000) * (rand() % 4) );
    uint32_t serialized_length = 1 + long_string_length + ( ( long_string_length / 0xFFFF ) * 3 );
    if ( ( long_string_length % 0xFFFF ) > 0 )
    {
        serialized_length += 3;
    }
    char long_string[long_string_length];
    
    for (index = 0; index < long_string_length-1; index++) 
    {
        long_string[index] = 33 + (rand() % 93); // ascii range 33-126
    }
    long_string[long_string_length-1] = 0x00;

    char serialized[serialized_length];
    serialized[0] = PROTO_STRING;

    for (index = 0; index < long_string_length; index += 0xFFFF)
    {
        window_width = 0xFFFF;
        if (index + 0xFFFF >= long_string_length)
        {
            last_window = 1;
            window_width = long_string_length - index;
        }     
        serialized[index+window_offset+3] = last_window;
        memcpy(serialized+index+4+window_offset,long_string+index,window_width);
        window_width = htobe16(window_width);
        memcpy(serialized+index+1+window_offset,&window_width,2);
        window_offset += 3;
    }

    proto_string_t sample_empty_string = {
        .length = long_string_length,
        .string = long_string // note, this string length means it is not null-terminated
    };

    proto_item_node_t random_string = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    serialize_and_check_object( &random_string, serialized, serialized_length, 1);
}

void serialize_max_window_string()
{
    print_test_name();    
    uint32_t long_string_length = 0x10000;
    uint32_t serialized_length = long_string_length-1+4;
    char long_string[long_string_length];
    
    uint64_t index;
    for (index = 0; index < long_string_length-1; index++) 
    {
        long_string[index] = 0x42;
    }
    long_string[long_string_length-1] = 0x00;
    char serialized[serialized_length];
    memcpy(serialized,"\x02\xFF\xFF\x01",4);

    memcpy(serialized+4,long_string,long_string_length);

    proto_string_t sample_empty_string = {
        .length = long_string_length-1,
        .string = long_string // note, this string length means it is not null-terminated
    };

    proto_item_node_t max_window_string = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    serialize_and_check_object( &max_window_string, serialized, serialized_length, 1);
}

void serialize_small_window_string()
{
    print_test_name();
    char serialized[26] = "\x02\x00\x16\x01This is a test string.";

    proto_string_t sample_empty_string = {
        .length = 22,
        .string = "This is a test string." // note, this string length means it is not null-terminated
    };

    proto_item_node_t small_window_string = {
        .type = PROTO_STRING,
        .value.string_p = &sample_empty_string
    };
    serialize_and_check_object( &small_window_string, serialized, 26, 1);
}

void serialize_string(void)
{
    printf("\n");
    serialize_empty_string();
    serialize_small_window_string();
    serialize_max_window_string();
    serialize_multi_window_randow_string();

    printf("\tTest Result: ");

}


void serialize_empty_array()
{
    print_test_name();
    char serialized[4] =   "\x03"
                           "\x01\xFF";

    proto_array_t sample_data_array_array_serialized = {
        .size = 0,
        .nodes = NULL
    };

    proto_item_node_t sample_data_array_serialized = {
        .type = PROTO_ARRAY,
        .value.array_p = &sample_data_array_array_serialized
    };


    serialize_and_check_object( &sample_data_array_serialized, serialized, 3, 1);
}

void serialize_nested_array()
{
    print_test_name();
    char serialized[53] =   "\x03"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                "\x00\x03"
                                "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                "\x00\x02\x00\x05\x01test."
                                "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                            "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01";

    proto_string_t sample_string_obj = {
        .length = 5,
        .string = "test." // note, this string length means it is not null-terminated
    };

    proto_item_node_t sample_data_array_nodes_inner[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_STRING, .value.string_p = &sample_string_obj},
        { .type = PROTO_UINT64, .value.uint64 = 1 }

    };

    proto_array_t sample_data_array_array_inner = {
        .size = 3,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes_inner
    };

     proto_item_node_t sample_data_array_nodes[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_ARRAY, .value.array_p = &sample_data_array_array_inner },
        { .type = PROTO_UINT64, .value.uint64 = 1 }
    };

    proto_array_t sample_data_array_array = {
        .size = 3,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes
    };

    proto_item_node_t sample_data_array = {
        .type = PROTO_ARRAY,
        .value.array_p = & sample_data_array_array
    };


    serialize_and_check_object( &sample_data_array, serialized, 53, 1);
}

void serialize_simple_array()
{
    print_test_name();
    char serialized[41] =   "\x03"
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

    proto_item_node_t sample_data_array = {
        .type = PROTO_ARRAY,
        .value.array_p = & sample_data_array_array
    };


    serialize_and_check_object( &sample_data_array, serialized, 41, 1);
}

void deserialize_simple_array()
{
    print_test_name();
    char serialized[41] =   "\x03"
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

    proto_item_node_t sample_data_array = {
        .type = PROTO_ARRAY,
        .value.array_p = & sample_data_array_array
    };

    deserialize_and_check_object(serialized, 41, &sample_data_array);
}

void deserialize_empty_array()
{
    print_test_name();
    char serialized[4] =   "\x03"
                           "\x01\xFF";


    proto_array_t sample_data_array_array_serialized = {
        .size = 0,
        .nodes = NULL
    };

    proto_item_node_t sample_data_array_serialized = {
        .type = PROTO_ARRAY,
        .value.array_p = &sample_data_array_array_serialized
    };


    deserialize_and_check_object(  serialized, 3, &sample_data_array_serialized);
}

void deserialize_nested_array()
{
    print_test_name();
    char serialized[53] =   "\x03"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                "\x00\x03"
                                "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                                "\x00\x02\x00\x05\x01test."
                                "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                            "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01";

    proto_string_t sample_string_obj = {
        .length = 5,
        .string = "test." // note, this string length means it is not null-terminated
    };

    proto_item_node_t sample_data_array_nodes_inner[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_STRING, .value.string_p = &sample_string_obj},
        { .type = PROTO_UINT64, .value.uint64 = 1 }

    };

    proto_array_t sample_data_array_array_inner = {
        .size = 3,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes_inner
    };

     proto_item_node_t sample_data_array_nodes[] = {
        { .type = PROTO_UINT64, .value.uint64 = 1 },
        { .type = PROTO_ARRAY, .value.array_p = &sample_data_array_array_inner },
        { .type = PROTO_UINT64, .value.uint64 = 1 }
    };

    proto_array_t sample_data_array_array = {
        .size = 3,
        .nodes = (proto_item_node_t *) &sample_data_array_nodes
    };

    proto_item_node_t sample_data_array = {
        .type = PROTO_ARRAY,
        .value.array_p = & sample_data_array_array
    };


    deserialize_and_check_object( serialized, 53, &sample_data_array);
}

void deserialize_empty_dict()
{
    print_test_name();
    char serialized[20] =   "\x04"
                            "\x01\xFF\xFF";

    proto_dict_t sample_data_dict_simple_array= {
        .size = 0,
        .items = NULL
    };

    proto_item_node_t sample_data_dict_simple = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_simple_array
    };

    deserialize_and_check_object( serialized, 4, &sample_data_dict_simple);
}

void deserialize_nested_dict()
{
    print_test_name();
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



    deserialize_and_check_object(  serialized, 111, &sample_data_dict);
    
}

void serialize_nested_dict()
{
    print_test_name();
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



    serialize_and_check_object( &sample_data_dict, serialized, 111, 1);
    
}

void serialize_empty_dict()
{
    print_test_name();
    char serialized[20] =   "\x04"
                            "\x01\xFF\xFF";

    proto_dict_t sample_data_dict_simple_array= {
        .size = 0,
        .items = NULL
    };

    proto_item_node_t sample_data_dict_simple = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_simple_array
    };
    serialize_and_check_object( &sample_data_dict_simple, serialized, 4, 1);
    
}

void serialize_simple_dict()
{
    print_test_name();
    char serialized[20] =   "\x04"
                            "\x01\x01\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\xab\xcd\xef\x11\x22\x33\x44\x55";

    proto_dict_item_t sample_data_dict_nodes_simple[] = {
        {
            .value = { .type = PROTO_UINT64, .value.uint64 = 0xabcdef1122334455 },
            .key = { .type = PROTO_INT64, .value.int64 = 0 }
        }
    };

    proto_dict_t sample_data_dict_simple_array= {
        .size = 1,
        .items = (proto_dict_item_t *) &sample_data_dict_nodes_simple
    };

    proto_item_node_t sample_data_dict_simple = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_simple_array
    };

    serialize_and_check_object( &sample_data_dict_simple, serialized, 20, 1);
}

void deserialize_simple_dict()
{
    print_test_name();
    char serialized[20] =   "\x04"
                            "\x01\x01\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\xab\xcd\xef\x11\x22\x33\x44\x55";

    proto_dict_item_t sample_data_dict_nodes_simple[] = {
        {
            .value = { .type = PROTO_UINT64, .value.uint64 = 0xabcdef1122334455 },
            .key = { .type = PROTO_INT64, .value.uint64 = 0 }
        }
    };

    proto_dict_t sample_data_dict_simple_array= {
        .size = 1,
        .items = (proto_dict_item_t *) &sample_data_dict_nodes_simple
    };

    proto_item_node_t sample_data_dict_simple = {
        .type = PROTO_DICT,
        .value.dict_p =  &sample_data_dict_simple_array
    };

    deserialize_and_check_object( serialized, 20, &sample_data_dict_simple);
}



void serialize_array(void)
{
    printf("\n");

    serialize_simple_array();
    
    serialize_empty_array();

    serialize_nested_array();

    printf("\tTest Result: ");

}

void deserialize_array(void)
{
    printf("\n");

    deserialize_simple_array();

    deserialize_empty_array();

    deserialize_nested_array();

    printf("\tTest Result: ");

}

void serialize_dict(void)
{
    printf("\n");

    serialize_simple_dict();

    serialize_empty_dict();

    serialize_nested_dict();

    printf("\tTest Result: ");
}

void deserialize_dict(void)
{
    printf("\n");

    deserialize_simple_dict();

    deserialize_empty_dict();

    deserialize_nested_dict();

    printf("\tTest Result: ");
}



#endif
