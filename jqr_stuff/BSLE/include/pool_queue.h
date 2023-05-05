#ifndef POOL_QUEUE_H
#define POOL_QUEUE_H

#include "server.h"

struct node {
    struct node *next;
    client_info *client_socket;
};

typedef struct node node_t;

void enqueue(client_info *client_socket);
client_info *dequeue();


#endif