#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Use Dijkstra's Algorithm to find the shortest distance from the source
 * to other vertices in the DIRECTED graph.
 * 
 * The graph will be provided as an adjacency matrix.
 *
 * Ex: {{1, 0}, {2, 0}} 
 *          A   B
 *      A   1   0
 *      B   2   0
 *
 *  A has a self edge of weight one and no edge to B
 *  B has an edge of weight 2 to A
 *
 * 
 * Your function should allocate memory that will be free'd by the caller.
 * 
 * If the are nodes that cannot be reached from the source, then the distance 
 * to those nodes should be UINT32_MAX.
 * 
 * If there is an error you should return a null pointer.
 * 
 * @param graph The graph
 * @param src The source vertex
 * @param num_vertices The number of vertices in the graph
 * @return uint32_t* Array of distances from the source to every node.
 */
uint32_t* dijkstra(uint32_t** graph, size_t src, size_t num_vertices);
