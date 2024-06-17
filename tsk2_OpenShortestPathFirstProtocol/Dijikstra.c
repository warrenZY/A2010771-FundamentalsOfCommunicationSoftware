#include "Dijikstra.h"
#include "Message.h"
#include <stdio.h>

#define myNew(className,Num) (className*)malloc(Num*sizeof(className));



void Dijkstra(Graph* _graph, int startVex, ShortPaths* _shortPaths) {

	for (int index = 0; index < _graph->VexNum; index++) {
		(_shortPaths + index)->Dist = MaxDistance;
		(_shortPaths + index)->Prev = -1;
		(_shortPaths + index)->Visited = 0;
	}

	(_shortPaths + startVex)->Dist = 0;


	int minVex = 0;
	for (int index = 0; index < _graph->VexNum; index++) {
		//find shortpath from none visit node
		int minDist = MaxDistance;
		
		for (int j = 0; j < _graph->VexNum; j++) {
			if ((!(_shortPaths + j)->Visited) && ((_shortPaths + j)->Dist < minDist)) {
				minDist = (_shortPaths + j)->Dist;
				minVex = j;
			}
		}
		(_shortPaths + minVex)->Visited = 1;



		ArcNode* _arcNode = _graph->adjList[minVex].FirstArc;
		while (_arcNode) {
			int adjVex = _arcNode->Adjvex;
			int weight = _arcNode->Weight;

			if ((!(_shortPaths + adjVex)->Visited) &&
				((_shortPaths + minVex)->Dist + weight < (_shortPaths + adjVex)->Dist)) {
				(_shortPaths + adjVex)->Dist = (_shortPaths + minVex)->Dist + weight;
				(_shortPaths + adjVex)->Prev = minVex;
			}
			_arcNode = _arcNode->NextArc;
			
			
		}
		
	}
}

int LocateVex(Graph* G,unsigned int Data){
	for (int i = 0; i < G->VexNum; i++) {
		if (G->adjList[i].Data == Data)
			return i;
	}
	return -1;
}

void PrintfALGraph(Graph*G) {
	for (int i = 0; i < G->VexNum; i++) {
		printf("%d.%d.%d.%d:\t", G->adjList[i].Data / 256 / 256 / 256,
			G->adjList[i].Data / 256 / 256 % 256,
			G->adjList[i].Data / 256 % 256,
			G->adjList[i].Data % 256);
		ArcNode* p = myNew(ArcNode, 1);
		p = G->adjList[i].FirstArc;
		while (p) {
			printf("%d ", p->Adjvex);
			p = p->NextArc;
		}
		printf("\n");
	}
	printf("%sComplete output list.\n\n", InformationMsg);
}

/*
void CreatALGraph(Graph* G) {
	int i, j, k;
	ArcNode* arcNode;
	printf("%splz type int number of Node and number of arc (int,int):", InformationMsg);
	scanf("%d,%d",&G->VexNum,&G->ArcNum);
	for (i = 0; i < G->VexNum; i++) {
		scanf(&G->adjList[i].Data);
		G->adjList[i].FirstArc = NULL;
	}
	for (k = 0; k < G->ArcNum; k++) {
		printf("%sinput Node number of arc i,j:\n",InformationMsg);
		scanf("%d,%d", &i, &j);
		arcNode = myNew(ArcNode, 1);
		arcNode->Adjvex = j;
		arcNode->NextArc = G->adjList[i].FirstArc;
		G->adjList[i].FirstArc = arcNode;
		arcNode= myNew(ArcNode, 1);
		arcNode->Adjvex = i;
		arcNode->NextArc = G->adjList[j].FirstArc;
		G->adjList[j].FirstArc = arcNode;
		//把每个顶点的若干个邻接点的边表节点变成一个链
	}
}


*/
