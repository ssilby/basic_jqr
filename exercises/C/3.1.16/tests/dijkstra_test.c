#ifndef JQR_3116_DIJKSTRA_TEST_H
#define JQR_3116_DIJKSTRA_TEST_H

#include "../include/dijkstra.h"

#include <CUnit/CUnit.h>
#include <CUnit/CUnitCI.h>

void
test_basic(void)
{
    size_t num_vertices = 9;
    uint32_t demo[9][9] = {
        {0, 4, 0, 0, 0, 0, 0, 8, 0},  {4, 0, 8, 0, 0, 0, 0, 11, 0},
        {0, 8, 0, 7, 0, 4, 0, 0, 2},  {0, 0, 7, 0, 9, 14, 0, 0, 0},
        {0, 0, 0, 9, 0, 10, 0, 0, 0}, {0, 0, 4, 14, 10, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 2, 0, 1, 6},  {8, 11, 0, 0, 0, 0, 1, 0, 7},
        {0, 0, 2, 0, 0, 0, 6, 7, 0}};

    uint32_t** graph = calloc(num_vertices, sizeof(uint32_t*));
    size_t i;
    size_t j;
    for (i = 0; i < num_vertices; i++)
    {
        graph[i] = calloc(num_vertices, sizeof(uint32_t));
        for (j = 0; j < num_vertices; j++)
        {
            graph[i][j] = demo[i][j];
        }
    }

    uint32_t* res = dijkstra(graph, 0, num_vertices);
    CU_ASSERT_PTR_NOT_NULL_FATAL(res);
    uint32_t expected[9] = {0, 4, 12, 19, 21, 11, 9, 8, 14};

    for (i = 0; i < num_vertices; i++)
    {
        CU_ASSERT_EQUAL(expected[i], res[i]);
    }

    free(res);
    for (i = 0; i < num_vertices; i++)
    {
        free(graph[i]);
    }
    free(graph);
}

void
test_null(void)
{
    uint32_t* res = dijkstra(NULL, 0, 9);
    CU_ASSERT_PTR_NULL(res);
}

void
test_one_node(void)
{
    size_t num_vertices = 1;
    uint32_t** graph = calloc(num_vertices, sizeof(uint32_t*));
    size_t i;
    for (i = 0; i < num_vertices; i++)
    {
        graph[i] = calloc(num_vertices, sizeof(uint32_t));
    }

    uint32_t* res = dijkstra(graph, 0, num_vertices);
    CU_ASSERT_PTR_NOT_NULL_FATAL(res);
    CU_ASSERT_EQUAL(res[0], 0);
    free(res);
    for (i = 0; i < num_vertices; i++)
    {
        free(graph[i]);
    }
    free(graph);
}

void
test_disconnect(void)
{
    size_t num_vertices = 3;
    uint32_t demo[3][3] = {{0, 3, 0}, {3, 2, 0}, {0, 2, 0}};

    uint32_t** graph = calloc(num_vertices, sizeof(uint32_t*));
    size_t i;
    size_t j;
    for (i = 0; i < num_vertices; i++)
    {
        graph[i] = calloc(num_vertices, sizeof(uint32_t));
        for (j = 0; j < num_vertices; j++)
        {
            graph[i][j] = demo[i][j];
        }
    }

    uint32_t* res = dijkstra(graph, 0, num_vertices);
    CU_ASSERT_PTR_NOT_NULL_FATAL(res);
    uint32_t expected[3] = {0, 3, UINT32_MAX};

    for (i = 0; i < num_vertices; i++)
    {
        CU_ASSERT_EQUAL(expected[i], res[i]);
    }

    free(res);
    for (i = 0; i < num_vertices; i++)
    {
        free(graph[i]);
    }
    free(graph);
}

#endif
