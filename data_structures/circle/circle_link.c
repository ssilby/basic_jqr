#include <stdio.h>

#include "circle_link.h"

/*
'Demonstrate skill in creating and using a circularly linked list that accepts any data type:

Creating a circularly linked list with n number of items
Navigating through a circularly linked list
Finding the first occurrence of an item in a circularly linked list
Sorting the circularly linked list alphanumerically using a function pointer
Removing selected items from the circularly linked list
Inserting an item into a specific location in a circularly linked list
Removing all items from the circularly linked list
Destroying a circularly linked list
*/

struct node
{
    void *data;
    struct node *next;
};

struct circle_list
{
    size_t curr_size;
    struct node *head;
    struct node *last;
};


c_list *
create_circle_list(void)
{
    c_list *clist = malloc(sizeof(*clist));
    if (!clist)
    {
        fprintf(stderr, "not enough memory\n");
        return NULL;
    }

    clist->curr_size = 0;
    clist->head = NULL;
    clist->last = NULL;
    
    return clist;
}


int
insert_front(c_list *list, void *data)
{
    if (!list || !data)
    {
        return -1;
    }

    struct node *node = malloc(sizeof(*node));
    if (!node)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }
    node->data = data;

    if (list->head == NULL)
    {
        node->next = node;
        list->head = node;
        list->last = node;
    }
    else
    {
        struct node *temp = list->head;
        list->head = node;
        list->head->next = temp;
        list->last->next = list->head;
    }

    list->curr_size++;

    return 0;
}

int
insert_back(c_list *list, void *data)
{
    if (!list || !data)
    {
        return -1;
    }

    struct node *node = malloc(sizeof(*node));
    if (!node)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }
    node->data = data;

    if (list->head == NULL)
    {
        node->next = node;
        list->head = node;
        list->last = node;
    }
    else
    {
        struct node *temp = list->last;
        list->last = node;
        temp->next = list->last;
        list->last->next = list->head;
    }

    list->curr_size++;

    return 0;
}


int
insert_at(c_list *list, void *data, size_t index)
{
    if (!list || !data || !index)
    {
        return -1;
    }

    if (index > list->curr_size)
    {
        fprintf(stderr, "index: %lu out of range\n", index);
    }

    if (index == 0)
    {
        insert_front(list, data);
    }
    else
    {
        struct node *new = malloc(sizeof(*new));
        if (!new)
        {
            fprintf(stderr, "not enough memory\n");
            return -1;
        }
        new->data = data;
        new->next = NULL;
        struct node *tmp = list->head;

        while (--index)
        {
            tmp = tmp->next;
        }
        new->next = tmp->next;
        tmp->next = new;
    }
    list->curr_size++;

    return 0;
}


void
show_front(c_list *list, print_it print_node)
{
    if (!list || list->curr_size == 0)
    {
        return;
    }
    print_node(list->head->data);

    return;
}


void
show_back(c_list *list, print_it print_node)
{
    if (!list || list->curr_size == 0)
    {
        return;
    }
    print_node(list->last->data);

    return;
}


void
show_at(c_list *list, print_it print_node, size_t index)
{
    if (!list || list->curr_size == 0)
    {
        return;
    }
    if (index > list->curr_size)
    {
        fprintf(stderr, "Index out of range");
        return;
    }
    struct node *n = list->head;
    for (size_t i = 0; i < index; ++i)
    {
        n = n->next;
    }
    print_node(n->data);
}


struct node *remove_front(c_list *list)
{
    struct node *tmp = list->head;
    list->head = tmp->next;
    list->last->next = tmp->next;
    tmp->next = NULL;
    list->curr_size--;

    return tmp;
}


struct node *remove_back(c_list *list)
{
    struct node *tmp = list->last;
    struct node *n = list->head;
    for (size_t i = 0; i < list->curr_size - 1; ++i)
    {
        n = n->next;
    }
    list->last = n;
    n->next = list->head;
    tmp->next = NULL;
    list->curr_size--;

    return tmp;
}


struct node *remove_at(c_list *list, size_t index)
{
    struct node *n = list->head;
    for (size_t i = 0; i < index - 1; ++i)
    {
        n = n->next;
    }

    struct node *tmp = n->next;

    n->next = tmp->next;
    tmp->next = NULL;
    list->curr_size--;

    return tmp;
}


void
view_list(c_list *list, print_it print_node)
{
    if (!list || list->curr_size == 0)
    {
        return;
    }

    struct node *n = list->head;
    for (size_t i = 0; i < list->curr_size; ++i)
    {
        print_node(n->data);
        n = n->next;
    }
    return;
}


struct node *
sorted_merge(struct node *a, struct node *b, comp_it comp)
{
    struct node* result = NULL;
 
    /* Base cases */
    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);
    /* Pick either a or b, and recur */
    // if a is greater than or equal to b
    if (comp(a->data, b->data) >= 0) {
        result = a;
        result->next = sorted_merge(a->next, b, comp);
    }
    else {
        result = b;
        result->next = sorted_merge(a, b->next, comp);
    }
    return (result);
}


void
split_list(struct node* source, struct node** frontRef, struct node** backRef)
{
    struct node* fast;
    struct node* slow;
    slow = source;
    fast = source->next;
 
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}


/* sorts the linked list by changing next pointers (not data) */
void
merge_sort(struct node** headRef, comp_it comp)
{
    struct node* head = *headRef;
    struct node* a;
    struct node* b;
 
    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
    /* Split head into 'a' and 'b' sublists */
    split_list(head, &a, &b);
 
    /* Recursively sort the sublists */
    merge_sort(&a, comp);
    merge_sort(&b, comp);
 
    /* answer = merge the two sorted lists together */
    *headRef = sorted_merge(a, b, comp);
}


void
sort_list(c_list *list, comp_it comp)
{
    if (!list || !comp)
    {
        return;
    }
    list->last->next = NULL;
    merge_sort(&list->head, comp);
    struct node *n = list->head;
    for (size_t i = 0; i < list->curr_size - 1; ++i)
    {
        n = n->next;
    }
    list->last = n;
    n->next = list->head;
}


void destroy_list(c_list *list, free_it free_data)
{
    if (!list || !free_data)
    {
        return;
    }
    struct node *n = list->head;
    for (size_t i = 0; i < list->curr_size; ++i)
    {
        free_data(n->data);
        struct node *tmp = n;
        n = n->next;
        free(tmp);
    }
    free(list);
}


