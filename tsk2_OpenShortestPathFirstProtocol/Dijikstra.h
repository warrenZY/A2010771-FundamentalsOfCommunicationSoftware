#ifndef __Dijikstra_H__
#define __Dijikstra_H__

#define MaxDistance 65535
#define MaxVex 9
#define MaxArc 16

typedef struct{
	int Adjvex;//node index
	struct ArcNode* NextArc;
	int Weight;
}ArcNode;

typedef struct{
	unsigned int Data;//node data
	//int InDegree;//node indegree
	struct ArcNode* FirstArc;//pointer of first node
}AdjList[MaxVex+1];

typedef struct{
	AdjList adjList;
	int VexNum, ArcNum;//number of node and arc
}Graph;

typedef struct{
	int Dist;
	int Prev;
	int Visited;
}ShortPaths;




void Dijkstra(Graph* _graph, int startVex, ShortPaths *_shortPaths[]);
int LocateVex(Graph* G, unsigned int Data);
void PrintfALGraph(Graph* G);
#endif // !__Dijikstra_H__
