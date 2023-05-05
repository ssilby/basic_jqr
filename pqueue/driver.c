#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "pqueue.h"


struct num {
    int val;
};

// jqr 4.1.15
int num_comp(void *x, void *y)
{
    struct num *a = (struct num *) x;
    struct num *b = (struct num *) y;
    return (a->val - b->val);
}

static bool test_int_enqueue(pqueue *pq, size_t n)
{
    bool ret = true;
    printf("Enqueueing %lu random numbers\n", n);
    for (size_t i = 0; i < n; ++i) {
        struct num *r = malloc(sizeof(*r));
        if (!r) {
            fprintf(stderr, "Driver failed to create alloc\n");
            return false;
        }
        r->val = rand() % 8192;
        printf("Enqueing %d\n", r->val);
        if (!enqueue(pq, r)) {
            fprintf(stderr, "FAIL: Enqueue failed!\n");
            ret = false;
        }
    }
    return ret;
}

static bool test_int_dequeue(pqueue *pq, size_t n)
{
    bool ret = true;
    int prev = INT_MAX;
    printf("Dequeuing %lu values\n", n);
    for (size_t i = 0; i < n; ++i) {
        void *val = dequeue(pq);
        if (!val) {
            ret = false;
            fprintf(stderr, "FAIL: Null value dequeued\n");
            break;
        }
        struct num *curr = ((struct num *) val);
        printf("Dequeued %d\n", curr->val);
        if (curr->val > prev) {
            ret = false;
            fprintf(stderr, "FAIL: Values are not in correct order\n");
        }
        prev = curr->val;
        free(curr);
    }
    return ret;
}

static bool test_int_pq(pqueue *pq)
{
    if (!pq) {
        return false;
    }
    srand(time(NULL));
    // Random number in range 5 to 20
    size_t n = rand() % (20 - 4) + 5;
    bool pass = true;
    if (!test_int_enqueue(pq, n)) {
        pass = false;
    } else if (!test_int_dequeue(pq, n)) {
        pass = false;
    }
    if (!pass) {
        return pass;
    }
    printf("Dequeuing from empty pqueue\n");
    void *val = dequeue(pq);
    if (val) {
        pass = false;
    }
    // Random number in range 10 to 30
    n = rand() % (30 - 9) + 10;
    printf("Testing partial dequeue for memory leaks\n");
    // Only Dequeue half of the enqueued data leaving the rest to be
    // freed by the destroy function
    if (!test_int_enqueue(pq, n)) {
        pass = false;
    } else if (!test_int_dequeue(pq, n / 2)) {
        pass = false;
    }
    return pass;
}

static bool fail_cases(void)
{
    bool ret = false;
    printf("Attempting to create priority queue with size of zero\n");
    pqueue *zero_pq = create_pqueue(num_comp, 0, free);
    if (zero_pq) {
        fprintf(stderr, "Created pq of size 0\n");
        destroy_pqueue(&zero_pq);
    }
    pqueue *pq = create_pqueue(num_comp, 5, free);
    if (!pq) {
        fprintf(stderr, "Unable to create test pq\n");
        return true;
    }
    printf("Created pqueue of size 5\n");
    printf("Attempting to enqueue NULL data\n");
    if (enqueue(pq, NULL)) {
        fprintf(stderr, "NULL data was enqueued\n");
        ret = true;
        dequeue(pq);
    }
    for (int i = 0; i < 9; ++i) {
        struct num *n = malloc(sizeof *n);
        if (!n) {
            fprintf(stderr, "Driver alloc failed\n");
            return ret;
        }
        n->val = i;
        bool queued = enqueue(pq, n);
        if (!queued) {
            if ( i < 5) {
                ret = true;
                fprintf(stderr, "Enqueue failed before max\n");
            }
            free(n);
        } else {
            if (i >= 5) {
                ret = true;
                fprintf(stderr, "Enqueue returned true when queueing more than max\n");
            }
        }
    }
    destroy_pqueue(&pq);
    return ret;
}

int main(void)
{
    pqueue *num_pq = create_pqueue(num_comp, 40, free);
    bool pass = test_int_pq(num_pq);
    if (!pass) {
        fprintf(stderr, "Failed int priority queue.\n");
    }
    if (fail_cases()) {
        fprintf(stderr, "Failed edge cases tests\n");
    }
    destroy_pqueue(&num_pq);
}
