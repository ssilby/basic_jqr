#ifndef AVL_H
#define AVL_H

#include <stdbool.h>

struct Node
{
    void *data;
    struct Node *left;
    struct Node *right;
    int height;
};

typedef int (*comp_it)(void *, void *);
typedef void (*free_it)(void *);
typedef void *(*get_it)(void *);

typedef struct Tree avl_tree;

avl_tree *create_avl_tree(comp_it comp, free_it free_data, get_it get_data);

int max(int a, int b);
int insert_node(avl_tree *tree, void *data);
int remove_node(avl_tree *tree, void *data);
void preOrder(struct Node *root, get_it get_data);
bool search(avl_tree *tree, void *data);
void destroy_tree(avl_tree *tree);

#endif