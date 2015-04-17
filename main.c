#include <stdio.h>
#include "bool.h"
#include "graph.h"

int main(void)
{
    graph g;
	int i;

	read_graph(&g,TRUE);
	print_graph(&g);
    return 0;
}