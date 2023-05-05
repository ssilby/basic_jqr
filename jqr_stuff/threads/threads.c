#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define THREAD_POOL_SIZE 20

pthread_t thread_pool[THREAD_POOL_SIZE];

struct node {
    struct node *next;
    int *client_socket;
}


void *thread_function(void *arg);

int main(int argc, char **argv)
{

    // create a bunch of threads for future connections
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }
}


void enqueue(int *client_socket)
{
    node_t *newnode = malloc(sizeof(node_t))
}

dequeue