#ifndef PQEUEU_H
#define PQEUEU_H

#include <stdbool.h>

typedef struct pqueue_ pqueue;
typedef int (*compare_p)(void *, void *);
typedef void (*free_p)(void *);
typedef void (*str_f)(void *, char *);

pqueue *create_pqueue(compare_p compare, size_t size, free_p free_it);


bool destroy_pqueue(pqueue **);

bool enqueue(pqueue *, void *);

void *dequeue(pqueue *);

int qsize(pqueue *);

void *peek(pqueue *);

void printq(pqueue *, str_f);

#endif