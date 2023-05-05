/**
* All code contained in this file was adapted from Liam Echlin
*/

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include "thpool.h"
#include "pool_queue.h"

struct pool
{
    size_t numWorkers;
    queue_t *work;

    pthread_t *tids;
    size_t tid_sz;
};

static void *
END_WORK(void *unused)
{
    (void)unused;
    return NULL;
}

void *
worker(void *incoming)
{
    queue_t *q = incoming;
    for(;;) {
        work_t func;
        void *arg;
        Queue_dequeue(q, &func, &arg);


        if (func == END_WORK) {
            return NULL;
        } else if (func) {
            func(arg);
        } else {
            sleep(1);
        }
    }
}

Pool *
Pool_create(size_t numWorkers)
{
    Pool *p = malloc(sizeof(*p));
    if (!p) {
        return NULL;
    }

    p->work = Queue_create();
    if (!p->work) {
        free(p);
        return NULL;
    }

    p->tids = malloc(sizeof(*p->tids) * numWorkers);
    if (!p->tids) {
        free(p->work);
        free(p);
        return NULL;
    }

    p->numWorkers = numWorkers;
    for (size_t n=0; n < p->numWorkers; ++n) {
        pthread_t tid;
        pthread_create(&tid, NULL, worker, p->work);
        p->tids[n] = tid;
    }

    return p;
}

bool
Pool_addTask(Pool *p, work_t t, void *arg)
{
    if (!p) {
        return false;
    }

    return Queue_enqueue(p->work, t, arg);
}

void
Pool_wait(Pool *p)
{
    if (!p) {
        return;
    }

    for (size_t n=0; n < p->numWorkers; ++n) {
        Queue_enqueue(p->work, END_WORK, NULL);
    }

    for (size_t n=0; n < p->numWorkers; ++n) {
        pthread_join(p->tids[n], NULL);
    }
}

void
Pool_destroy(Pool *p)
{
    if (!p) {
        return;
    }

    Queue_destroy(p->work);
    free(p->tids);
    free(p);
}
