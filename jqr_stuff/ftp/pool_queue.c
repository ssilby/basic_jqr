#include <stdlib.h>
#include <stdio.h>

#include "pool_queue.h"
#include "server.h"


node_t *head = NULL;
node_t *tail = NULL;

void 
enqueue(client_info *client_socket)
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->client_socket = client_socket;
    newnode->next = NULL;
    if (tail == NULL) 
    {
        head = newnode;
    } 
    else 
    {
        tail->next = newnode;
    }
    tail = newnode;
}


// returns NULL if queue is empty
// returns pointer to client socket if not
client_info *
dequeue()
{
    if (head == NULL) 
    {
        return NULL;
    } 
    else 
    {
        client_info *result = head->client_socket;
        node_t *temp = head;
        head = head->next;
        if (head == NULL) 
        {
            tail = NULL;
        }
        free(temp);
        return result;
    }
}