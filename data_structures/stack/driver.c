#include <stdio.h>

#include "stack.h"


void
free_data(void *data)
{
    int *tmp = (int *) data; 
    printf("%d freed\n", *tmp);
}


void *
get_data(void *data)
{
    int *converted = (int *) data;
    return converted;
}


int
main(void)
{
    stack *s = create_stack(free_data, get_data);

    if (!s)
    {
        printf("stack not created :(\n");
    }
    else
    {
        printf("stack created!\n");
    }

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int e = 5;


    push(s, (void *) &a);
    push(s, (void *) &b);
    push(s, (void *) &c);
    push(s, (void *) &d);
    push(s, (void *) &e);

    int *x = (int *) find(s, 3);
    printf("the third item is %d\n", *x);

    int *n = (int *) pop_nth(s, 3);
    printf("3rd item popped: %d\n", *n);

    printf("stack size after 5 push and one pop: %lu\n", s->size);

    int *z = (int *) peek(s);
    printf("peek!: %d\n", *z);

    int *p = (int *) pop(s);

    printf("stack size after one pop: %lu\n", s->size);
    printf("popped value 2 = %d\n", *p);

    int *pp = (int *) pop(s);

    printf("stack size after two pop: %lu\n", s->size);
    printf("popped value = %d\n", *pp);


    s->free_data(n);
    s->free_data(p);
    s->free_data(pp);

    destroy_stack(s);

    return 0;
}