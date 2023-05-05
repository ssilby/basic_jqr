#ifndef GRAPH_H
#define GRAPH_H

// Defining the structures required for graphs
// Creating a graph with n number of nodes
// Adding n number of edges to a graph
// Finding a node within an existing graph
// Finding an edge within a graph
// Remove an edge from a graph
// Remove a node and all of its edges from a graph
// Calculate the weight of a path within a graph
// Destroy the graph

typedef void (*free_it)(void *);
typedef int (*comp_it)(void *, void *);


// A structure to represent an adjacency list node
struct AdjListNode 
{
    int dest;
    int weight;
    void *data;
    struct AdjListNode* next;
};
 
// A structure to represent an adjacency list
struct AdjList 
{
    struct AdjListNode* head;
};
 
// Graph stores an array of adjlist nodes with V number of vertices
struct Graph 
{
    int V;
    struct AdjList* array;
    free_it free_data;
    comp_it comp_data;
};


struct Graph* createGraph(int V, free_it free_data, comp_it comp_data);
void addEdge(struct Graph* graph, int src, int dest, void *src_data, void *des_data, int weight);
void printGraph(struct Graph* graph);
int find_node(struct Graph *graph, void *data);
int find_edge(struct Graph *graph, int src, int dest);
void remove_node(struct Graph *graph, int node);
//find_path
//destroy_graph


#endif