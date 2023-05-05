#include <stdlib.h>

typedef void (*free_it)(void *);
typedef void *(*get_it)(void *);

typedef struct Stack stack;

struct Node 
{
    void *data;
    struct Node *next;
};

struct Stack
{
    size_t size;
    struct Node *root;
    get_it get_data;
    free_it free_data;
};

stack *create_stack(free_it free_data, get_it get_data);
struct Node *new_node(void *data);
int push(stack *s, void *data);
void * pop(stack *s);
void *peek(stack *s);
void *find(stack *s, size_t n);
void * pop_nth(stack *s, size_t n);
void destroy_stack(stack *s);
/*

'Demonstrate skill in creating and using a stack that accepts any data type:

Navigating through a stack to find the nth item
Adding an item in a stack
Removing selected items from a stack
Removing all items from the stack
Destroying a stack
Preventing a stack overrun
*/