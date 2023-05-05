#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "dijkstra.h"

void
free_data(void *data)
{
    int *tmp = (int *) data; 
    printf("%d freed\n", *tmp);
}


int
compare(void *a, void *b)
{
    char *atmp = (char *)a;
    char *btmp = (char *)b;
    int res = strcmp(atmp, btmp);
    return res;
}


int main()
{
    // create the graph given in above fugure
    int V = 5;

    struct Graph* graph = createGraph(V, free_data, compare);

    const char *zero = "rei";
    const char *one = "ichi";
    const char *two = "ni";
    const char *three = "san";
    const char *four = "yon";

    const char *bad = "doesn't exist";

    addEdge(graph, 0, 1, (void *) zero, (void *) one, 1);
    addEdge(graph, 0, 4, (void *) zero, (void *) four, 2);
    addEdge(graph, 1, 2, (void *) one, (void *) two, 3);
    addEdge(graph, 1, 3, (void *) one, (void *) three, 4);
    addEdge(graph, 1, 4, (void *) one, (void *) four, 5);
    addEdge(graph, 2, 3, (void *) two, (void *) three, 6);
    addEdge(graph, 3, 4, (void *) three, (void *) four, 7);

    int ret = find_node(graph, (void *) zero);

    (ret == 0) ? puts("exists") : puts("doesn't exist");

    ret = find_node(graph, (void *) one);

    (ret == 0) ? puts("exists") : puts("doesn't exist");

    ret = find_node(graph, (void *) two);

    (ret == 0) ? puts("exists") : puts("doesn't exist");

    ret = find_node(graph, (void *) three);

    (ret == 0) ? puts("exists") : puts("doesn't exist");

    ret = find_node(graph, (void *) four);

    (ret == 0) ? puts("exists") : puts("doesn't exist");

    ret = find_node(graph, (void *) bad);

    (ret == 0) ? puts("exists") : puts("doesn't exist");
 
    // print the adjacency list representation of the above
    // graph
    printGraph(graph);

    int exists = find_edge(graph, 1, 3);
    (exists == 0) ? puts("1,3 edge exists") : puts("1,3 edge doesn't exist");
 
    exists = find_edge(graph, 2, 4);
    (exists == 0) ? puts("2,4 edge exists") : puts("2,4 edge doesn't exist");

    dijkstra(graph, 0, 2);

    remove_node(graph, 3);

    printGraph(graph);

    dijkstra(graph, 0, 2);
    
    destroy_graph(graph);


    return 0;
}
