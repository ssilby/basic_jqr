#include "circle_link.h"
#include <string.h>

struct node
{
    void *data;
    struct node *next;
};

void print_list(void *data)
{
    if (!data)
    {
        return;
    }
    char *words;
    words = (char *) data;
    printf("%s\n", words);
}

int
compare(void *a, void *b)
{
    char *atmp = (char *)a;
    char *btmp = (char *)b;
    int res = strcmp(atmp, btmp);
    return res;
}

void
free_data(void *data)
{
    printf("%s freed!\n", (char *) data);
}

int main(void)
{
    c_list *list = create_circle_list();

    char *one = (char *)"a";
    char *two = (char *) "potato";
    char *three = (char *) "flew around";
    char *last = (char *) "the room";
    char *before_last = (char *) "it's gross and I don't like it";

    insert_front(list, one);
    insert_front(list, two);
    insert_front(list, three);

    insert_back(list, last);
    insert_at(list, before_last, 1);

    view_list(list, print_list);
    puts("");

    puts("sorted list:");
    sort_list(list, compare);
    view_list(list, print_list);
    puts("");
    puts("last:");
    show_back(list, print_list);

    // printf("front: ");
    // show_front(list, print_list);
    // printf("back: ");
    // show_back(list, print_list);
    // printf("index 3: ");
    // show_at(list, print_list, 3);

    // struct node *n = remove_front(list);
    // printf("front removed: ");
    // print_list(n->data);
    // puts("new list:");
    // view_list(list, print_list);
    // puts("");
    // puts("remove last: ");
    // struct node *b = remove_back(list);
    // print_list(b->data);
    // puts("list after last removed: ");
    // view_list(list, print_list);
    // puts("");

    // struct node *i = remove_at(list, 1);
    // puts("removed at index 1: ");
    // print_list(i->data);
    // puts("");
    // puts("list after last removed: ");
    // view_list(list, print_list);
    // puts("");


    // free(n);
    // free(b);
    // free(i);
    destroy_list(list, free_data);
}