#include <stdio.h>

#include "stack.h"



stack *
create_stack(free_it free_data, get_it get_data)
{
    stack *s = malloc(sizeof(*s));
    if (!s)
    {
        fprintf(stderr, "not enough memory\n");
        return NULL;
    }

    s->root = NULL;
    s->size = 0;
    s->free_data = free_data;
    s->get_data = get_data;

    return s;
}


struct Node *
new_node(void *data)
{
    if (!data)
    {
        return NULL;
    }
    struct Node *node = malloc(sizeof(*node));
    if (!node)
    {
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    return node;
}


int
push(stack *s, void *data)
{
    struct Node *node = new_node(data);
    if (!node)
    {
        return -1;
    }
    node->next = s->root;
    s->root = node;
    s->size++;

    return 0;
}


void *
pop(stack *s)
{
    if (s->size == 0)
    {
        return NULL;
    }
    struct Node *tmp = s->root;
    s->root = s->root->next;
    void *popped_data = tmp->data;
    free(tmp);

    s->size--;

    return popped_data;
}


void *
peek(stack *s)
{
    if (!s || s->size == 0)
    {
        return NULL;
    }

    return s->root->data;
}


void *
find(stack *s, size_t n)
{
    if (!s || s->size == 0)
    {
        return NULL;
    }
    if (n > s->size)
    {
        fprintf(stderr, "Index out of range");
        return NULL;
    }
    struct Node *node = s->root;
    for (size_t i = 0; i < n - 1; ++i)
    {
        node = node->next;
    }
    return node->data;
}


void *
pop_nth(stack *s, size_t n)
{
    if (!s || s->size == 0)
    {
        return NULL;
    }
    if (n > s->size)
    {
        fprintf(stderr, "Index out of range");
        return NULL;
    }
    struct Node *node = s->root;
    for (size_t i = 0; i < n - 2; ++i)
    {
        node = node->next;
    }
    struct Node *tmp = node->next;
    node->next = tmp->next;

    void *tmp_data = tmp->data;
    free(tmp);

    s->size--;

    return tmp_data;
}


//destroys and frees entire stack
void
destroy_stack(stack *s)
{
    if (!s || s->size == 0)
    {
        return;
    }
    struct Node *n = s->root;
    for (size_t i = 0; i < s->size; ++i)
    {
        s->free_data(n->data);
        struct Node *tmp = n;
        n = n->next;
        free(tmp);
    }
    free(s);
}