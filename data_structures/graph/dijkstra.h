// adapted from geeksforgeeks.com
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdlib.h>
#include "graph.h"


void dijkstra(struct Graph* graph, size_t src, int dest);
void print_path(int *path, int src, int dest);
void remove_node(struct Graph *graph, int node);
void destroy_graph(struct Graph *graph);


#endif