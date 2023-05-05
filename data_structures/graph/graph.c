// adapted from geeksforgeeks.com

#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>


#include "graph.h"



// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, void *data, int weight)
{
    struct AdjListNode* newNode
        = (struct AdjListNode*)malloc(
            sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->data = data;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}
 
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V, free_it free_data, comp_it comp_data)
{
    struct Graph *graph = malloc(sizeof(*graph));
    graph->V = V;
    graph->free_data = free_data;
    graph->comp_data = comp_data;
 
    // Create an array of adjacency lists.  Size of
    // array will be V
    graph->array = (struct AdjList*)malloc(
        V * sizeof(struct AdjList));
 
    // Initialize each adjacency list as empty by
    // making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}
 
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, void* src_data, void *des_data, int weight)
{
    // Add an edge from src to dest.  A new node is
    // added to the adjacency list of src.  The node
    // is added at the beginning
    struct AdjListNode* check = NULL;
    struct AdjListNode* newNode = newAdjListNode(dest, des_data, weight);
 
    if (graph->array[src].head == NULL) {
        newNode->next = graph->array[src].head;
        graph->array[src].head = newNode;
    }
    else {
 
        check = graph->array[src].head;
        while (check->next != NULL) {
            check = check->next;
        }
        // graph->array[src].head = newNode;
        check->next = newNode;
    }
 
    // Since graph is undirected, add an edge from
    // dest to src also
    newNode = newAdjListNode(src, src_data, weight);
    if (graph->array[dest].head == NULL) {
        newNode->next = graph->array[dest].head;
        graph->array[dest].head = newNode;
    }
    else {
        check = graph->array[dest].head;
        while (check->next != NULL) {
            check = check->next;
        }
        check->next = newNode;
    }
 
    // newNode = newAdjListNode(src);
    // newNode->next = graph->array[dest].head;
    // graph->array[dest].head = newNode;
}
 
// A utility function to print the adjacency list
// representation of graph
void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->V; ++v) {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n head ", v);
        while (pCrawl) {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}
 

// A function to see if a node with certain data exists
int find_node(struct Graph *graph, void *data)
{
    if (!graph || !data)
    {
        return -1;
    }

    for (int i = 0; i < graph->V; ++i)
    {
        struct AdjListNode *tmp = graph->array[i].head;
        if (tmp->data == NULL)
        {
            continue;
        }
        else
        {
            while (tmp != NULL)
            {
                if (graph->comp_data(tmp->data, data) == 0)
                {
                    return 0;
                }
                else
                {
                    tmp = tmp->next;
                }
            }
        }
    }
    return -1;
} 


// check if an edge exists in the graph
int find_edge(struct Graph *graph, int src, int dest)
{
    if (!graph)
    {
        return -1;
    }
    if (src < 0 || src >= graph->V || dest < 0 || dest >= graph->V)
    {
        return -1;
    }
    struct AdjListNode *tmp = graph->array[src].head;
    if (tmp == NULL)
    {
        return -1;
    }
    else
    {
        while(tmp != NULL)
        {
            if (tmp->dest == dest)
            {
                return 0;
            }
            else
            {
                tmp = tmp->next;
            }
        }
    }
    return -1;
}


// remove a node from the graph
void
remove_node(struct Graph *graph, int node)
{
    if (!graph)
    {
        return;
    }
    if (node < 0 || node >= graph->V)
    {
        return;
    }    

    if (graph->array[node].head == NULL)
    {
        return;
    }

    for (int i = 0; i < graph->V; ++i)
    {   
        if (i == node)
        {
            struct AdjListNode *current = graph->array[i].head;
            struct AdjListNode *next;
            while (current != NULL)
            {
                next = current->next;
                graph->free_data(current->data);
                free(current);
                current = next;
            }
            graph->array[i].head = NULL;
        }
        else
        {
            struct AdjListNode *current = graph->array[i].head;
            if (!current)
            {
                continue;
            }
            if (current->dest == node)
            {
                graph->array[i].head = current->next;
                graph->free_data(current->data);
                free(current);
                current = NULL;
                continue;
            }
            while (current->next != NULL)
            {
                if (current->next->dest == node)
                {
                    struct AdjListNode *tmp = current->next;
                    current->next = current->next->next;
                    graph->free_data(tmp->data);
                    free(tmp);
                    tmp = NULL;
                    break;
                }
                current = current->next;
            }
        }
    }
    //graph->V--;
}

void
destroy_graph(struct Graph *graph)
{
    if (!graph)
    {
        return;
    }

    for (int i = 0; i < graph->V; ++i)
    {
        remove_node(graph, i);
    }
    free(graph->array);
    free(graph);
    graph = NULL;
}