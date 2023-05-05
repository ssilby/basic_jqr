#ifndef JQR_82_SERIALIZE_H
#define JQR_82_SERIALIZE_H

#include <stdint.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


/*test*/
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define PROTO_UINT64 0x00
#define PROTO_INT64  0x01
#define PROTO_STRING 0x02
#define PROTO_ARRAY  0x03
#define PROTO_DICT   0x04
#define PROTO_NULL   0xFF


typedef struct _proto_array proto_array_t;
typedef struct _proto_dict proto_dict_t;


typedef struct _proto_string 
{
    uint64_t length;
    char * string;
} proto_string_t;

typedef union _proto_item 
{
    uint64_t uint64;
    int64_t  int64;
    proto_string_t *string_p;
    proto_array_t  *array_p;
    proto_dict_t   *dict_p;
} proto_item_t;

typedef struct _proto_item_node
{
    uint8_t type;
    proto_item_t value;
} proto_item_node_t;

struct _proto_array
{
    uint64_t size;
    proto_item_node_t * nodes; // NULL for empty array
}; 

typedef struct _proto_dict_item
{
    proto_item_node_t key;
    proto_item_node_t value;
} proto_dict_item_t;

struct _proto_dict
{
    uint64_t size;
    proto_dict_item_t * items;
};

proto_item_node_t * proto_deserialize(int fd);

uint8_t proto_serialize(int fd, proto_item_node_t *item);

#endif
