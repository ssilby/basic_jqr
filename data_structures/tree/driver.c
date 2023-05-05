#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "avl.h"

struct Tree
{
    struct Node *root;
    size_t size;
    comp_it comp;
    free_it free_data;
    get_it get_data;
};


int comp(void *a, void *b)
{
    int *atmp = (int *) a;
    int *btmp = (int *) b;
    return  *atmp - *btmp;
}

void free_data(void *data)
{
    int *tmp = (int *) data; 
    printf("%d freed\n", *tmp);
}

void *get_data(void *data)
{
    int *converted = (int *) data;
    return converted;
}

int
main(void)
{
    avl_tree *tree = create_avl_tree(comp, free_data, get_data);
    if (tree)
    {
        puts("tree created!");
    }

    /* Constructing tree given in the above figure */
    int t = 10, q = 20, w = 30, e = 40, r = 50, m = 25;
    insert_node(tree, &t);
    insert_node(tree, &q);
    insert_node(tree, &w);
    insert_node(tree, &e);
    insert_node(tree, &r);
    insert_node(tree, &m);

    /* The constructed AVL Tree would be
            30
            /  \
            20   40
        /  \     \
        10  25    50
    */

    printf("Preorder traversal of the constructed AVL"
            " tree is \n");
    preOrder(tree->root, tree->get_data);
    puts("");
    bool yes = search(tree, &t);
    int z = 99;
    bool no = search(tree, &z);
    if (yes == true)
    {
        printf("10 is in tree\n");
    }
    else
    {
        printf("10 isn't in tree\n");
    }
    if (no == true)
    {
        printf("99 is in tree\n");
    }
    else
    {
        printf("99 isn't in tree\n");
    }
    //Preorder traversal of the constructed AVL tree is
    //30 20 10 25 40 50
    remove_node(tree, &e);
    remove_node(tree, &r);
    preOrder(tree->root, tree->get_data);
    puts("");

    destroy_tree(tree);
    
    return 0;
}