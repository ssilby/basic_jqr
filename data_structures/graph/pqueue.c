#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pqueue.h"
#define MAX 100



/**
 * creates and initializes a pqueue struct
 * 
 * takes a function pointer and the max size of the priority queue
 * 
 * returns an initialized pqueue struct
 */
pqueue *create_pqueue(compare_p compare, size_t size, free_p free_it, decrease_k decrease)
{
    // if no function pointer passed, error to stderr and return null
    if (!compare) {
        fprintf(stderr, "invalid arguement\n");
        return NULL;
    }

    // create struct on heap and initialize to 0
    pqueue *pq = malloc(sizeof(*pq));
    if (!pq) {
        return NULL;
    }
    pq->size = 0;
    pq->max_size = size;
    pq->queue = malloc(sizeof(*(pq->queue)) * size);
    if (!pq->queue) {
        return NULL;
    }
    // add compare function pointer to pqueue struct
    pq->comp = compare;
    pq->free = free_it;
    pq->decrease = decrease;

    return pq;
}

/**
 * adds a void pointer to the priority queue in proper order
 * 
 * takes the pqueue struct and void pointer as parameters
 * 
 * returns a bool of true if the pointer was successfully added
 * or false if not
 */
bool enqueue(pqueue* pq, void *data)
{
    // if no struct passed to function return false with error
    if (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return false;
    }
    // return value for compare function
    int res;

    // if no void pointer passed to function return false with error
    if (!data) {
        return false;
    }

    // if first item in queue, add to queue and set size to 0
    if (pq->size == 0) {
        pq->queue[0] = data;
        pq->size += 1;
    } else {
        if (pq->size == pq->max_size - 1) {
            pq->max_size = pq->max_size * 2;
            pq->queue = realloc(pq->queue, sizeof(void *) * pq->max_size);
        }
        pq->queue[pq->size] = data;
        pq->size += 1;

        // loop through queue and compare new data to each item
        // if data is less than 0, switch values, otherwise return
        for (int i = pq->size - 1; i > 0; i--) {
            res = pq->comp(data, pq->queue[i - 1]);
            if (res >= 0) {
                return true;
            } else {
                pq->queue[i] = pq->queue[i - 1];
                pq->queue[i - 1] = data;
            } 
        }
        
    }
    return true;
}

/**
 * returns the size of the priority queue
 */
int qsize(pqueue *pq)
{
    if (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return 0;
    }
    return pq->size;
}

/**
 * prints out the items in the queue if they point to integers
 */
void printq(pqueue *pq, str_f stringify)
{
    if (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return;
    }
    char *string = malloc(20);
    if (!string) {
        return;
    }
    if (pq->size > 0) {
        for (size_t i = 0; i < pq->size; i++) {
            memset(string, '\0', 20);
            stringify(pq->queue[i], string);
            printf("%s\n", string);
        }
        free(string);
    }
    return;

}

/**
 * frees the queue arrays and structs 
 */
bool destroy_pqueue(pqueue **pq)
{
    if (!pq || !*pq) {
        fprintf(stderr, "invalid arguement\n");
        return false;
    } 
    if ((*pq)->free) {
        for (size_t i = 0; i < (*pq)->size; ++i) {
            (*pq)->free((*pq)->queue[i]);
        }
    }

    free((*pq)->queue);
    free(*pq);
    *pq = NULL;

    return true;
}

/**
 * grabs the top of the pqueue and decreases the pqueue size by one
 * 
 * takes pqueue struct pointer as an argument
 * 
 * returns void pointer of the top item in the pqueue
 */
void *dequeue(pqueue *pq)
{
    void *top;
    if  (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return NULL;
    } else if (pq->size == 0){
        return NULL;
    } else {
        top = pq->queue[pq->size - 1];
        pq->queue[pq->size - 1] = NULL;
        pq->size -= 1;
    }
    return top;
}

/**
 * Looks at the top value in the priority queue
 * 
 * takes a pqueue struct pointer as an argument
 * 
 * returns the top void pointer of the pqueue or
 * NULL if the queue is empty
 */
void *peek(pqueue *pq)
{
    void *top;
    if  (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return NULL;
    // if the queue is empty return NULL
    } else if (pq->size == 0){
        return NULL;
    } else {
        // grabs the last item in the queue array
        top = pq->queue[pq->size - 1];
    }
    return top;
}


bool
find_data(pqueue *pq, void *data)
{
    if (!data)
    {
        return false;
    }
    if  (!pq) {
        fprintf(stderr, "invalid arguement\n");
        return false;
    } else if (pq->size == 0){
        return false;
    } else {
        int res;
        for (size_t i = 0; i < pq->size; ++i)
        {
            res = pq->comp(data, pq->queue[i]);
            if (res == 0)
            {
                return true;
            }
        }
    }
    return false;
}

