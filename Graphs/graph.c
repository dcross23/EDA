#include "graph.h"

#include <stdio.h>
#include "./OtherFiles/linkedListQueue.h"
#include "./OtherFiles/linkedListStack.h"

#define COLOR_TEXT(text) "\x1b[36;1m"#text"\x1b[0m"


/**
 * Initiates the graph with default values.
 */
void initGraph(Graph *g){
	int i,j;
	GraphEdge *e;

	for(i=1; i<=g->order; i++){
		g->vertices[i].visited      = 0;
		g->vertices[i].entryDegree  = 0; 
		g->vertices[i].topSort      = 0;
    		g->vertices[i].distance     = INF;
    		g->vertices[i].weight       = INF;
    		g->vertices[i].previous     = 0; 
	}

	for(i=1; i<=g->order; i++){
		e = g->vertices[i].edges;
	
		while(e!=NULL){
			j = e->vertex;
			(g->vertices[j].entryDegree)++;
			e = e->next;			
		}
	}
}


/**
 * Depth First Search. 
 * Starting from a vertex (firstVertex parameter), the graph is traversed
 *  until there are no more available ways, returning back to next node that
 *  can be reached (first node in first vertex edges list), and repeat
 *  the same process recursively.
 *
 * IMPORTANT!!! It depends on the first node choosen and the type of 
 *  graph (directed or not directed), so some vertices may not be visited.
 *
 * (Is similar to backtracking, starting from a node, travels the graph
 *  and return back when it can not advance more.
 */
void dfs(int firstVertex, Graph *g){ 
	GraphEdge *e;

	printf("%d ",firstVertex);
	g->vertices[firstVertex].visited = 1;
	
	e = g->vertices[firstVertex].edges;
	while (e != NULL){ 
		if (!g->vertices[e->vertex].visited)
			dfs(e->vertex, g);

		e = e->next;
	}
}

/**
 * Solves the problem of DFS when some vertices are not visited.
 * This improvement causes BFS to execute for every not visited
 *  vertex until all of them are visited. 
 *
 * This algorithm makes DFS executable for unconnected graphs.
 */
void fullDfs(int firstVertex, Graph *g){
	int i;
	
	initGraph(g);
	dfs(firstVertex, g);
	
	for(i=1; i<=g->order; i++){
		if(!(g->vertices[i].visited))
			dfs(i, g);
	}
}



/**
 * Breadth First Search. 
 * Starting from a vertex (firstVertex parameter), the graph is traversed
 *  until there are no more adyacent vertices. This is, for every reachable
 *  vertex, the algorithm will visit recursively the adyacent vertices 
 *  until it reaches a node with no more adyacent unvisited vertices.
 *
 * IMPORTANT!!! It depends on the first node choosen and the type of 
 *  graph (directed or not directed), so some vertices may not be visited.
 */
void bfs(int firstVertex, Graph *g){ 
	int v;
	GraphEdge *e;
	LLQueue q;
	createEmptyQueue(&q);
	
	queueAdd(firstVertex, &q);
	while (!isQueueEmpty(&q)){
		queueRemove(&q, &v);
		
		if (!g->vertices[v].visited) {
			printf("%d ",v);
			g->vertices[v].visited=1;
			
			e = g->vertices[v].edges;
			while (e != NULL){
				queueAdd(e->vertex, &q);
				e = e->next;
			}
		}
	}
}

/**
 * Solves the problem of BFS when some vertices are not visited.
 * This improvement causes BFS to execute for every not visited
 *  vertex until all of them are visited. 
 *
 * This algorithm makes BFS executable for unconnected graphs.
 */
void fullBfs(int firstVertex, Graph *g){
	int i;

	initGraph(g);	
	bfs(firstVertex, g);
	
	for(i=1; i<=g->order; i++){
		if(!(g->vertices[i].visited))
			bfs(i, g);
	}
}




/**
 * Algorith that only works for directed acyclic graphs.
 * Topological sort of a directed graph is a linear order of its vertices
 *  where for each edge uv from vertex u to vertex v, u comes before v
 *  in the ordering.
 * If the graph is  (u->v), topological sort will be "u v" (u before v).
 *
 *  Search vertex with entry degree 0 --> O(1) best case, O(n) worst case --> O(n)
 *  This search vertex loop is executed for each vertex (outside loop) --> O(n²)
 * 
 *  Algorithm time complexity --> O(n) best case, O(n²) worst case --> O(n²)
 */
int topologicalSort(Graph *g){
	int i,j;
	int v;
	GraphEdge *e;
	
	initGraph(g);
	
	for(i=1; i<=g->order; i++){
		//Searches for a vertex with entry degree 0
		v = -1;	
		for(j=1; j<=g->order; j++){
			if(g->vertices[j].entryDegree==0 && g->vertices[j].topSort==0){
				v = j;
				//If we put here a break, the result might be different
				// but also correct
			}
		}
		
		//If there is any vertex with entry degree 0, there is
		// a cycle (is a cyclic graph)
		if(v == -1) return -1;
			
		g->vertices[v].topSort = i;
		e = g->vertices[v].edges;
		while(e != NULL){
			(g->vertices[e->vertex].entryDegree)--;
			e = e->next;
		}
	}
	return 0;
}


/**
 * The algorithm does the same as topologicalSort algorithm, but now it will use a queue
 *  to improve time complexity.
 *  
 * Add to/Remove from queue each vertex --> O(n)
 * Inner loop executes 1 time for each edge --> assuming "a" edges --> O(a)
 *
 * Now, algorithm time complexity is --> O(n+a)
 */
int improvedTopSort(Graph *g){
	int v, nextVertex;
	GraphEdge *e;
	LLQueue q;
	createEmptyQueue(&q);
	
	initGraph(g);

	//Insert in the queue all the elements with entry degree 0
	for(v=1; v<=g->order; v++){
		if(g->vertices[v].entryDegree == 0)
			queueAdd(v, &q);
	}
	
	//If there is any vertex with entry degree 0, there is
	// a cycle (is a cyclic graph)
	if(isQueueEmpty(&q)) return -1;
	
	nextVertex = 1;
	while(!isQueueEmpty(&q)){
		queueRemove(&q, &v);
		g->vertices[v].topSort = nextVertex++;
		
		e = g->vertices[v].edges;
		while(e != NULL){
			if(--(g->vertices[e->vertex].entryDegree) == 0){
				queueAdd(e->vertex, &q);
			}
			e = e->next;
		}
	}	

	
}




/**
 * From an initial vertex (firstVertex), it gets the shortest path to the rest
 *  of vertices of the no weighted graph. Because the graph is a no weighted, 
 *  it only matters the number of edges (distance) needed to reach a specific 
 *  vertex.
 * 
 * Loop for each vertex to asign each one a distance from firstVertex --> O(n)
 * Loop for each vertex searching a path from the firstVertex --> O(n)
 * 
 * Inner loop is executed n times for each distance that has to be asigned to 
 *  each vertex --> algorith complexity is --> O(n²)
 */
void npPathsAlgorithm(int firstVertex, Graph *g){
	int actualDist, v;
	GraphEdge *e;

	initGraph(g);
	g->vertices[firstVertex].distance = 0;
	g->vertices[firstVertex].previous = 0;
	
	for(actualDist=0; actualDist<g->order; actualDist++){
		for(v=1; v<=g->order; v++){
			if(!g->vertices[v].visited && g->vertices[v].distance == actualDist){
				g->vertices[v].visited = 1;
				e = g->vertices[v].edges;
				while(e != NULL){
					if(g->vertices[e->vertex].distance == INF){
						g->vertices[e->vertex].distance = g->vertices[v].distance + 1;
						g->vertices[e->vertex].previous = v;
					}
					e = e->next;
				}
			}
		}
	}
}




/**
 * The algorithm does the same as npPathsAlgorithm algorithm, but now it will use a queue
 *  to improve time complexity.
 *
 * Add to/Remove from queue each vertex --> O(n) in the worst case (all vertices are visited)
 * Inner loop executes 1 time for each edge --> assuming "a" edges --> O(a)
 *  (visited edges are not inserted in the queue)
 *
 * Now, algorithm time complexity is --> O(n+a)
 */
void improvedNpPathsAlg(int firstVertex, Graph *g){
	int v;
	GraphEdge *e;
	LLQueue q;
	
	initGraph(g);
	g->vertices[firstVertex].distance = 0;
	g->vertices[firstVertex].previous = 0;
	
	createEmptyQueue(&q);
	queueAdd(firstVertex, &q);
	while(!isQueueEmpty(&q)){
		queueRemove(&q, &v);
		g->vertices[v].visited = 1;
		
		e = g->vertices[v].edges;
		while(e != NULL){
			if(g->vertices[e->vertex].distance == INF){
				g->vertices[e->vertex].distance = g->vertices[v].distance + 1;
				g->vertices[e->vertex].previous = v;
				queueAdd(e->vertex, &q);
			}
			e = e->next;
		}
	}
}




/**
 * Algorithm to interpret the graph table and print the path from firstVertex
 *  to lastVertex. It returns the distance(cost) between the vertices.
 */
int seeCostAndPath(int firstVertex, int lastVertex, Graph *g){
	int v, p;
	LLStack s;
	createEmptyStack(&s);
	
	v = lastVertex;
	p = g->vertices[v].previous;
	push(lastVertex, &s);
	while(p != 0){
		push(p, &s);
		v = p;
		p = g->vertices[v].previous;
	}
	
	if(v != firstVertex){
		return INF;
	
	}else{
		while(!isStackEmpty(&s)){
			pop(&s, &v);
			printf("\x1b[35;1m%d \x1b[0m", v);
		}
		return g->vertices[lastVertex].distance;
	}
}


/**
 * Using seeCostAndPath algorithm, it prints all possible paths from firstVertex
 *  to the rest, including the distance(cost) between them.
 */
void seeAllCostsAndPaths(int firstVertex, Graph *g){
	int i, dist;
	for(i=1; i<=g->order; i++){
		if(i != firstVertex){
			printf("\t");
			dist = seeCostAndPath(firstVertex, i, g);
			if(dist != INF) printf(" ->  Path with cost \x1b[32;1m%d\x1b[0m  ->  from %d to %d \n", dist, firstVertex, i);
			else		printf("Theres no path from %d to %d \n", firstVertex, i);
		}
	}
}



/**
 * Prints the graph as a table with all necesary info.
 */
void printGraph(Graph *g){  
	int i;
	GraphEdge *e;

	printf("       %s \n", COLOR_TEXT(vis entD topS dist weight prev edges));
	printf("     %s \n", COLOR_TEXT(+----+----+----+----+------+----+----->));

	for(i=1;i<=g->order;i++){  
		printf("  %2d %s %2d %s %2d %s %2d %s", 
		       i, COLOR_TEXT(|),   g->vertices[i].visited, COLOR_TEXT(|),
		       g->vertices[i].entryDegree, COLOR_TEXT(|),
		       g->vertices[i].topSort, COLOR_TEXT(|));

		if (g->vertices[i].distance == INF) 
			printf("  * %s",COLOR_TEXT(|));
		else   
			printf(" %2d %s", g->vertices[i].distance, COLOR_TEXT(|));

		if (g->vertices[i].weight == INF) 
			printf("  *   %s",COLOR_TEXT(|));
		else  
			printf(" %2d   %s", g->vertices[i].weight, COLOR_TEXT(|));


		printf(" %2d %s",g->vertices[i].previous, COLOR_TEXT(|));
		
		e = g->vertices[i].edges;
		while (e != NULL){   
			#ifndef WEIGHTED_GRAPHS
  				printf(" ->%2d", e->vertex);	// Not weighted graph
			#else
				printf(" ->%2d, %2d", e->vertex, e->weight); // Weighted graph
			#endif 
			e = e->next;
		}
		printf("\n");
	}

	printf("     %s", COLOR_TEXT(+----+----+----+----+------+----+----->));
	printf("\n\x1b[36;1m\tGraph order = %d\x1b[0m\n",g->order);
}
