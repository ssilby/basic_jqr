#ifndef JQR_38_HELPER_FUNCTIONS_C
#define JQR_38_HELPER_FUNCTIONS_C


#include "helper_functions.h"



static char test_fd_filename[] = "/test_file";




int create_test_fd()
{
    int test_fd = shm_open(test_fd_filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (-1 == test_fd) 
    {
        err("failed to allocate shm fd");
    }
    return test_fd;
}




/**
 * @brief checks if to buffers are equal up to given length
 * 
 * @param first - pointer to first buffer
 * @param second - pointer to second buffer
 * @param length - number of bytes to check
 * @return uint8_t - returns 1 if they are equal, or 0 if they are not
 */
uint8_t equal_memory(char * first, char * second, uint64_t length) 
{
    uint64_t index = 0;
    uint8_t equal = 1;
    if (NULL == first || NULL == second )
    {
        return 0;
    }
    while (index < length)
    {
        if (first[index] != second[index])
        {
            equal = 0;
            break; // NOTE: not a cryptographically secure comparison
        }
        index++;
    }

    return equal;
}

uint8_t equal_objects(proto_item_node_t * first, proto_item_node_t * second) 
{
    uint8_t return_value = 1 ;
    uint64_t index = 0;
    if (NULL == first || NULL == second)
    {
        return first == second;
    }
    if (first->type != second->type)
    {
        return 0;
    }
    switch (first->type)
    {
        case PROTO_UINT64 :
            return_value = first->value.uint64 == second->value.uint64;
            break;
        
        case PROTO_INT64 :
            return_value = first->value.int64 == second->value.int64;
            break;

        case PROTO_STRING :
            if (NULL == first->value.string_p || NULL == second->value.string_p)
            {
                return first->value.string_p == second->value.string_p;
            }
            if (first->value.string_p->length != second->value.string_p->length)
            {
                return 0;
            }
            if (NULL == first->value.string_p->string || NULL == second->value.string_p->string)
            {
                return (first->value.string_p->string == second->value.string_p->string);
            }
            return equal_memory(first->value.string_p->string, second->value.string_p->string, second->value.string_p->length);
            break;

        case PROTO_ARRAY :
            if (NULL == first->value.array_p || NULL == second->value.array_p)
            {
                return first->value.array_p == second->value.array_p;
            }
            if (first->value.array_p->size != second->value.array_p->size)
            {
                return 0;
            }   
            if (NULL == first->value.array_p->nodes || NULL == second->value.array_p->nodes)
            {
                return first->value.array_p->nodes == second->value.array_p->nodes;
            }

            for(index = 0; index < first->value.array_p->size; index ++)
            {
                return_value &= equal_objects(& (first->value.array_p->nodes[index]), &(second->value.array_p->nodes[index]));
                if ( 0 == return_value )
                {
                    break;
                }
            }

            break;
        
        case PROTO_DICT :
            if (NULL == first->value.dict_p || NULL == second->value.dict_p)
            {
                return first->value.dict_p == second->value.dict_p;
            }
            if (first->value.dict_p->size != second->value.dict_p->size)
            {
                return 0;
            }   
            if (NULL == first->value.dict_p->items || NULL == second->value.dict_p->items)
            {
                return first->value.dict_p->items == second->value.dict_p->items;
            }

     
            for(index = 0; index < first->value.dict_p->size; index ++)
            {
                return_value &= equal_objects(&(first->value.dict_p->items[index].key), &(second->value.dict_p->items[index].key));
                return_value &= equal_objects(&(first->value.dict_p->items[index].value), &(second->value.dict_p->items[index].value));
                if ( 0 == return_value )
                {
                    break;
                }
            }

            break;

    }
    return return_value;
}

/**
 * @brief prints a given buffer as hex
 * 
 * @param buffer - the buffer to print
 * @param buffer_length - the number of bytes in the buffer.
 */
void print_buffer_as_hex(char * buffer, uint64_t in_buffer_length)
{
    uint64_t index = 0;
    uint64_t buffer_length = in_buffer_length;
    if (100 < buffer_length) {
        buffer_length = 100;
    }
    while ( index < buffer_length) {
        printf ("%02x",buffer[index] & 0x00FF);
        index++;
    }
    if (in_buffer_length > buffer_length)
    {
        printf("...");
    }
    printf("\n");
}

/**
 * @brief Get the contents from fd object
 * 
 * @param fd - file descriptor to get the contents of
 * @param dest - pointer to the pointer to store the response. This function will allocate memory and store it at this pointer.
 * @return uint64_t - returns the number of bytes read from the file. 
 */
uint64_t get_contents_from_fd(int fd, char ** dest)
{
    char c= 0;
    uint64_t index = 0;
    uint64_t buffer_length = lseek(fd,0,SEEK_END);
    ssize_t read_result = 0;
    if ( -1 == buffer_length)
    {
        err("cannot seek test file descriptor");
    }

    *dest = calloc(1,buffer_length);

    if (NULL == *dest)
    {
        err("cannot calloc buffer for fd contents");
    }
    lseek(fd,0,SEEK_SET);

    read_result = read(fd,*dest,buffer_length);

    if (read_result != buffer_length)
    {
        err("did not read entire length from fd");
    }    

    return buffer_length;
}
int8_t write_data_to_fd(int fd, const void * data_buffer, uint64_t data_buffer_length) 
{
    ssize_t send_count = 0;
    int8_t return_value = 0;

    send_count = write(fd, data_buffer, data_buffer_length);
    if (-1 == send_count)
    {
        return_value = -1;
    } else if ( send_count != data_buffer_length ) 
    {
        return_value = write_data_to_fd(fd, data_buffer + send_count, data_buffer_length - send_count);
    }

    return return_value;
}

void close_test_fd(int fd)
{
    close(fd);
    shm_unlink(test_fd_filename);
}


void print_proto_item(proto_item_node_t * item, uint32_t depth)
{
    char * nesting = NULL;
    if(NULL == item)
    {
        printf("\tNULL\n");
        return;
    }
    nesting = calloc(1,depth+1);
    memset(nesting,'\t',depth);
    switch (item->type)
    {
        case PROTO_UINT64:

            printf("%s\tType:\tUnsigned 64bit Integer\n",nesting);
            printf("%s\tValue:\t%lu\n",nesting,  item->value.uint64);

            break;
        
        case PROTO_INT64:

            printf("%s\tType:\tSigned 64bit Integer\n",nesting);
            printf("%s\tValue:\t%ld\n",nesting, item->value.int64);

            break;

        case PROTO_STRING:

            printf("%s\tType:\tString\n",nesting);
            if (NULL == item->value.string_p || NULL == item->value.string_p->string)
            {
                printf("%s\tValue:\tNULL Pointer\n",nesting);
            } 
            else
            {
                char * padded_string = calloc(1,item->value.string_p->length + 1);
                memcpy(padded_string,item->value.string_p->string,item->value.string_p->length);
                if ( 103 < item->value.string_p->length ) // truncate strings longer than 100
                {
                    padded_string[100] = '.';
                    padded_string[101] = '.';
                    padded_string[102] = '.';
                    padded_string[103] = '\0';
                }
                printf("%s\tValue ([%ld]|%%s|): |%s|\n",nesting,item->value.string_p->length,padded_string);
                free(padded_string);
            }
            
            break;

        case PROTO_ARRAY:
            printf("%s\tType:\tArray\n",nesting);
            if (NULL == item->value.array_p || NULL == item->value.array_p->nodes) 
            {
                printf("%s\tValue:\tNULL Pointer\n",nesting);
            }
            else
            {
                printf("%s\tSize:%ld\n",nesting,item->value.array_p->size);
                uint64_t index = 0;
                for (index = 0; index<item->value.array_p->size; index++)
                {
                    printf("%s\tItem %ld\n",nesting,index);
                    print_proto_item(item->value.array_p->nodes + index, depth + 1);
                    printf("\n");
                }
            }
            
            break;

        case PROTO_DICT:
            printf("%s\tType:\tDictionary\n",nesting);
            if (NULL == item->value.dict_p || NULL == item->value.dict_p->items) 
            {
                printf("%s\tValue:\tNULL Pointer\n",nesting);
            }
            else
            {
                printf("%s\tLength:%ld\n",nesting,item->value.dict_p->size);
                uint64_t index = 0;
                for (index = 0; index<item->value.dict_p->size; index++)
                {
                    printf("%s\tItem %ld\n",nesting,index);
                    printf("%s\tKey:\n",nesting);
                    print_proto_item(&(item->value.dict_p->items[index].key), depth + 1);
                    printf("%s\tValue:\n",nesting);
                    print_proto_item(&(item->value.dict_p->items[index].value), depth + 1);
                    printf("\n");
                }
            }
            break;

        case PROTO_NULL:

            break;

        default:
            printf("%s\t- Unsupported type: %d",nesting,item->type);
            break;

    }
    free(nesting);
}

void err(char * msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}


void cleanup_proto_item_node_t(proto_item_node_t * obj) 
{
    free_proto_item_node_objects(obj);

    free(obj);
}

void free_proto_string_t(proto_string_t *string_p)
{
    if(string_p != NULL)
    {
        free(string_p->string);
        string_p->string = NULL;
        free(string_p);
    }
}

void free_proto_item_node_array(proto_item_node_t *array, uint64_t size) 
{
    if (NULL == array)
    {
        return;
    }
    uint64_t index;
    for (index = 0; index < size; index++)
    {
        free_proto_item_node_objects(&(array[index]));
    }
}

void free_proto_array_t(proto_array_t *array_p)
{
    if(NULL != array_p )
    {
        if(NULL != array_p->nodes)
        {
            free_proto_item_node_array(array_p->nodes, array_p->size);
            free(array_p->nodes);
        }
        free(array_p);
    }

}

void free_proto_dict_array(proto_dict_item_t *array, uint64_t size) 
{
    if(NULL == array)
    {
        return;
    }
    uint64_t index;
    for (index = 0; index < size; index++)
    {   
        free_proto_item_node_objects(&array[index].key);
        free_proto_item_node_objects(&array[index].value);
    }
}

void free_proto_dict_t(proto_dict_t *dict_p)
{
    if( NULL != dict_p )
    {
        if (NULL != dict_p->items)
        {
            free_proto_dict_array(dict_p->items, dict_p->size);
            free(dict_p->items);
        }
        free(dict_p);
    }
}

void free_proto_item_node_objects(proto_item_node_t * node) 
{
    if (NULL != node)
    {
        if(PROTO_STRING == node->type) 
        {
            free_proto_string_t(node->value.string_p);
            node->value.string_p = NULL;
        }
        else if (PROTO_ARRAY == node->type)
        {
            free_proto_array_t(node->value.array_p);
            node->value.array_p = NULL;
        }
        else if (PROTO_DICT == node->type)
        {
            free_proto_dict_t(node->value.dict_p);
            node->value.dict_p = NULL;
        }
    }
   
}



void deserialize_and_check_object(char * data_in, uint64_t data_in_length, proto_item_node_t * expected_object) 
{
    int fd = create_test_fd();
    uint8_t result = 0;
    proto_item_node_t *new_object = NULL;
    lseek(fd,0,SEEK_SET);
    int8_t successful_write = write_data_to_fd(fd, data_in, data_in_length);
    if (-1 == successful_write)
    {
        err("unable to write to fd");
    }
    lseek(fd,0,SEEK_SET);

    new_object = proto_deserialize(fd);
    CU_ASSERT_PTR_NOT_NULL(new_object);
    result = equal_objects(new_object,expected_object);
    CU_ASSERT_EQUAL(result,1);
    if ( 1 != result )
    {
        printf("\n\tYour Code Created:\n");
        print_proto_item(new_object,1);
        printf("\tTest Cases Expected:\n");
        print_proto_item(expected_object,1);
    }
    cleanup_proto_item_node_t(new_object);
    close_test_fd(fd);
}


void serialize_and_check_object(proto_item_node_t * object, char * expected_data, uint64_t expected_data_length, uint8_t show_data)
{
    int fd = create_test_fd();
    char * fd_contents = NULL;
    uint64_t fd_contents_length = 0;
    uint8_t result = 0;

    lseek(fd,0,SEEK_SET);

    result = proto_serialize(fd,object);

    CU_ASSERT_EQUAL(result,0);
    
    fd_contents_length = get_contents_from_fd(fd,&fd_contents);

    if (fd_contents_length != expected_data_length)
    {
        printf("\n\tExpected Length:\t%ld\n\tActual Length:\t\t%ld",expected_data_length,fd_contents_length);
    }
    CU_ASSERT_EQUAL(fd_contents_length,expected_data_length);
    result = equal_memory(fd_contents,expected_data,expected_data_length);
    CU_ASSERT_EQUAL(result,1);

    if ( 1 == show_data && result != 1)
    {
        printf("\n\tExpected:\t");
        print_buffer_as_hex(expected_data,expected_data_length);
        printf("\tActual:\t\t");
        print_buffer_as_hex(fd_contents,fd_contents_length);

    }
    free(fd_contents);
    close_test_fd(fd);
}

#endif