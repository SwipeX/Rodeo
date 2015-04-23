/*	rodeo.c
    Adapted from strong.c
	Identify strongly connected components in a graph

	by: Steven Skiena
	begun: March 27, 2002
*/

/*
Copyright 2003 by Steven S. Skiena; all rights reserved. 

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/

/**
 * @author {Timothy Dekker, Ryan Serva}
 */


#include <stdio.h>
#include "bool.h"
#include "graph.h"
#include "stack.h"

bool processed[];
/* which vertices have been processed */
bool discovered[];
/* which vertices have been found */
int parent[];
/* discovery relation */

int entry_time[];
/* time of vertex entry */
int exit_time[];
/* time of vertex exit */

int low[MAXV + 1];
/* oldest vertex surely in component of v */
int scc[MAXV + 1];
/* strong component number for each vertex */

stack active;
/* active vertices of unassigned component */
int components_found;
/* number of strong components identified */

int components[99][99];
int currentcomponent = 0;

process_vertex_early(int v) {
    /*printf("entered vertex %d at time %d\n",v, entry_time[v]);*/
    push(&active, v);
}

process_vertex_late(int v) {
    /*printf("exit vertex %d at time %d\n",v, exit_time[v]);*/

    if (low[v] == v) {        /* edge (parent[v],v) cuts off scc */
/*printf("strong commonent started backtracking from %d\n",v);*/
        pop_component(v);
    }

    if (entry_time[low[v]] < entry_time[low[parent[v]]])
        low[parent[v]] = low[v];
}

pop_component(int v) {
    int t;                  /* vertex placeholder */

    components_found = components_found + 1;
    int i = 0;
    components[currentcomponent][i] = v;
    i++;
   // printf("%d is in component %d \n", v, components_found);

    scc[v] = components_found;
    while ((t = pop(&active)) != v) {
        scc[t] = components_found;

        components[currentcomponent][i] = t;
        i++;
       // printf("%d is in component %d with %d \n", t, components_found, v);
    }
    currentcomponent++;
}

process_edge(int x, int y) {
    int class;        /* edge class */

    class = edge_classification(x, y);
/*printf("(%d,%d) class %d\n", x,y,class);*/

    if (class == BACK) {
        if (entry_time[y] < entry_time[low[x]])
            low[x] = y;
/*printf("process BACK (%d,%d) low[%d]=%d  low[%d]=%d\n",x,y,x,low[x],y,low[y]);*/
    }

    if (class == CROSS) {
        if (scc[y] == -1)    /* component not yet assigned */

        if (entry_time[y] < entry_time[low[x]])
            low[x] = y;
/*printf("process CROSS (%d,%d) low[%d]=%d  low[%d]=%d\n",x,y,x,low[x],y,low[y]);*/
    }
}


void strong_components(graph *g) {
    int i;                /* counter */
    for (i = 1; i <= (g->nvertices); i++) {
        low[i] = i;
        scc[i] = -1;
    }
    components_found = 0;
    init_stack(&active);
    initialize_search(&g);
    for (i = 1; i <= (g->nvertices); i++)
        if (discovered[i] == FALSE) {
            dfs(g, i);
        /*pop_component(i);*/
        }
}
/**
 * Takes in a component index, and a vertex index.
 * Returns true if the component contains vertex, false otherwise.
 */
int inComponent(int component, int vertex) {
    if (component < 0 || vertex < 0)
        return FALSE;
    int *vertices;
    vertices = components[component];
    for (int i = 0; i < 99; i++) {
        if (vertices[i] == vertex)
            return TRUE;
    }
    return FALSE;
}

main() {
    int componentInDegree[99];
    int vertexInDegree[99];
    int vertexOutDegree[99];
    for (int j = 0; j < 99; j++) {
        vertexInDegree[j] = -1;
        vertexOutDegree[j] = -1;
        componentInDegree[j] = -1;

        for (int k = 0; k < 99; k++) {
            components[j][k] = -1;
        }
    }
    graph g;
    int i;

    read_graph(&g, TRUE);
    print_graph(&g);

    strong_components(&g);
    for (int j = 0; j < 5; j++) {
        int totalOut = 0;
        for (int k = 0; k < 5; k++) {
            int vert = components[j][k];
            if (vert < 0)
                continue;
            edgenode *edge = g.edges[components[j][k]];
            while (edge != NULL) {
                if (!inComponent(j, edge->y)) {
                    totalOut++;
                    if (vertexInDegree[edge->y] < 0)
                        vertexInDegree[edge->y]++;
                    vertexInDegree[edge->y]++;
                } else {
                    if (vertexInDegree[edge->y] < 0)
                        vertexInDegree[edge->y]++;
                }
                edge = edge->next;
            }
        }
        if (j < components_found)
            vertexOutDegree[j] = totalOut;
    }
    for (int j = 0; j < 99; j++) {
        if (vertexInDegree[j] > 0) {
            for (int g = 0; g < 99; g++) {
                if (inComponent(g, j)) {
                    componentInDegree[g] = 1;
                }
            }
        }
    }
    int out = 0;
    for (int j = 0; j < 99; j++) {
        if (vertexOutDegree[j] == 0)
            out++;
    }
    if (out == 1) {
        for (int j = 0; j < 99; j++) {
            if (vertexOutDegree[j] == 0) {
                printf("Sponges:");
                for (int g = 0; g < 99; g++) {
                    if (components[j][g] >= 0)
                        printf("%d ", components[j][g]);
                }
                printf("\n");
            }
        }
    }
    int in = 0;
    for (int j = 0; j < 99; j++) {
        if (componentInDegree[j] == -1 && j < components_found)
            in++;
    }
    if (in == 1) {
        for (int j = 0; j < 99; j++) {
            if (componentInDegree[j] == -1 && j < components_found) {
                printf("Bases:");
                for (int g = 0; g < 99; g++) {
                    if (components[j][g] >= 0)
                        printf("%d ", components[j][g]);
                }
                printf("\n");
            }
        }
    }
}
