// adapted from geeksforgeeks.com
// C / C++ program for Dijkstra's
// shortest path algorithm for adjacency
// list representation of graph
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "dijkstra.h"
#include "graph.h"
#include "pqueue.h"
 


// Structure to represent a min heap node
struct MinHeapNode
{
    size_t  v;
    int dist;
};


int
num_comp(void *x, void *y)
{
    struct MinHeapNode *a = (struct MinHeapNode *) x;
    struct MinHeapNode *b = (struct MinHeapNode *) y;
    return (a->dist - b->dist);
}


void
decrease(void *pq, size_t dest, size_t new_dist)
{
    if (!pq)
    {
        return;
    }
    pqueue *q = (pqueue *) pq;
    if (dest >= q->size || q->size == 0)
    {
        return;
    }

    for (size_t i = 0; i < q->size; ++i)
    {
        struct MinHeapNode *tmp = (struct MinHeapNode *) q->queue[i];
        if (tmp->v == dest)
        {
            tmp->dist = new_dist;
        }
    }
}


// A utility function to create a
// new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
           (struct MinHeapNode*)
      malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

 
// A utility function used to print the solution
void printArr(struct Graph *graph, int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
    {
        if (graph->array[i].head != NULL)
        {
            printf("%d \t\t %d\n", i, dist[i]);
        }
    }
}
 
// The main function that calculates
// distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void
dijkstra(struct Graph* graph, size_t src, int dest)
{
     
    // Get the number of vertices in graph
    size_t V = graph->V;

    // dist values used to pick
    // minimum weight edge in cut
    int *dist = malloc(sizeof(int) * V); 

    // create priority queue
    pqueue *pq = create_pqueue(num_comp, V, graph->free_data, decrease);
    
    struct MinHeapNode *node;
    // Initialize min heap with all
    // vertices. dist value of all vertices
    for (size_t v = 0; v < V; ++v)
    {
        if (graph->array[v].head == NULL)
        {
            continue;
        }
        if (v == src)
        {
            dist[v] = 0;
            node = newMinHeapNode(v, dist[v]);
        }
        else
        {
            dist[v] = INT_MAX;
            node = newMinHeapNode(v, dist[v]);
        }
        
        enqueue(pq, (void *) node);
    }
    
    // create parent array to find paths
    int *par = calloc(sizeof(int), graph->V);
    // set src parent to itself
    par[src] = src;
 
    // In the followin loop,
    // min heap contains all nodes
    // whose shortest distance
    // is not yet finalized.
    while (pq->size > 0)
    {
        // Extract the vertex with minimum distance value
        struct MinHeapNode* minHeapNode = (struct MinHeapNode *) dequeue(pq);
       
        // Store the extracted vertex number
        int u = minHeapNode->v;

        free(minHeapNode);
 
        // Traverse through all adjacent
        // vertices of u (the extracted
        // vertex) and update
        // their distance values
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;
 
            // If shortest distance to v is
            // not finalized yet, and distance to v
            // through u is less than its
            // previously calculated distance                    HERE make function to check is_in_pqueue
            if (find_data(pq, pCrawl) &&
                      dist[u] != INT_MAX &&
              pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;
 
                // update distance
                // value in min heap also
                pq->decrease(pq, v, dist[v]);               // replace the dest value in a node
                par[v] = u;
            }
            pCrawl = pCrawl->next;
        }
    }

    destroy_pqueue(&pq);
    // print the calculated shortest distances
    printArr(graph, dist, V);
    print_path(par, src, dest);

    free(dist);
    free(par);
}

void
print_path(int *path, int src, int dest)
{
    if (!path)
    {
        return;
    }
    int node = dest;
    printf("%d <- ", dest);
    while (path[node] != src)
    {
        printf("%d <- ", path[node]);
        node = path[node];
    }
    printf("%d\n", path[node]);
}