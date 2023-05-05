#ifndef CIRCLE_LINK_H
#define CIRCLE_LINK_H

#include <stdio.h>
#include <stdlib.h>

typedef struct circle_list c_list;

typedef int (*comp_it)(void *, void *);
typedef void (*free_it)(void *);
typedef void (print_it)(void *);

c_list *create_circle_list(void);

int insert_front(c_list *list, void *data);
int insert_back(c_list *list, void *data);
int insert_at(c_list *list, void *data, size_t index);

void show_front(c_list *list, print_it);
void show_back(c_list *list, print_it);
void show_at(c_list *list, print_it, size_t index);

struct node *remove_front(c_list *list);
struct node *remove_back(c_list *list);
struct node *remove_at(c_list *list, size_t n);

void view_list(c_list *list, print_it);
void sort_list(c_list *list, comp_it comp);
void destroy_list(c_list *list, free_it free_node);


#endif