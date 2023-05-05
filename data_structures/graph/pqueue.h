#ifndef PQEUEU_H
#define PQEUEU_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct pqueue_ pqueue;
typedef int (*compare_p)(void *, void *);
typedef void (*decrease_k)(void *, size_t, size_t);
typedef void (*free_p)(void *);
typedef void (*str_f)(void *, char *);


struct pqueue_ {
    size_t size;
    size_t max_size;
    void **queue;
    compare_p comp;
    decrease_k decrease;
    free_p free;
};


pqueue *create_pqueue(compare_p compare, size_t size, free_p free_it, decrease_k decrease);


bool destroy_pqueue(pqueue **);

bool enqueue(pqueue *, void *);

bool find_data(pqueue *, void *);

void *dequeue(pqueue *);

int qsize(pqueue *);

void *peek(pqueue *);

void printq(pqueue *, str_f);

bool find_data(pqueue *pq, void *data);

#endif