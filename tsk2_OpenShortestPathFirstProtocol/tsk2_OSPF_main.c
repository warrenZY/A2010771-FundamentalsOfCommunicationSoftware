#define _CRT_SECURE_NO_WARNINGS

/*
Creator: 2022210226-肖泽宇
程序实现链路状态信息的洪泛，构造通信网拓扑图；
利用Dijkstra算法，程序实现指定节点的路由转发表；
给出指定数据包路由目标节点。
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Dijikstra.h"
//#include "OspfPacket.h"
#include "Message.h"
#define myNew(className,Num) (className*)malloc(Num*sizeof(className));

Graph linkGraph;
ShortPaths shortPaths[MaxVex+1];
//FloodingInfo* floodInfo[MaxVex+1];

#define MaxDepth 5
unsigned int IpAddrs[] = {
	0xC0A80001,0xC0A80002,0xC0A80003,0xC0A80004,0xC0A80005,0xC0A80006
};


int RandomWake(int vexNum);
int CheckWake(int num[], int count);
int InitOspf(Graph* G, unsigned int ipAddrs[]);
int AddRouter(Graph* G);
void delay(int xms);





void main() {
START:
	printf("\r%sIniting OSPF.", InformationMsg);
	fflush(stdout);
	InitOspf(&linkGraph, IpAddrs);
	delay(1000);
	printf("\r%sIniting OSPF...Complete\n", InformationMsg);
	fflush(stdout);
	AddRouter(&linkGraph);
	printf("%sStart booting routers.\n", InformationMsg);
	PrintfALGraph(&linkGraph);
	/*
	unsigned int temp= 0xC0A80001;
	printf("%d.%d.%d.%d\n", temp / 256 / 256 / 256, temp / 256 / 256 % 256, temp / 256 % 256, temp % 256);
	*/

	int sourcePonit;
	//printf("please input start Router(using id): ");
	//scanf("%d", &sourcePonit);
	for (int sourcePonit = 1; sourcePonit < MaxVex+1; sourcePonit++) {
		Dijkstra(&linkGraph, sourcePonit, &shortPaths);
		printf("%sRouting Table via AdjList:\n", InformationMsg);
		printf("\033[47;30m%s%21s%9s%11s    \033[47;0m\n", "Source", "Destination", "Cost", "NextHop");
		for (int index = 0; index < MaxVex; index++) {
			if (shortPaths[index].Dist == 65535)
				continue;
			unsigned int curAddr = 0xC0A80000 + sourcePonit;
			unsigned int dstAddr = 0xC0A80000 + index;
			unsigned int nextHop = 0xC0A80000 + shortPaths[index].Prev;
			printf("%d.%d.%d.%d\t", curAddr / 256 / 256 / 256,
				curAddr / 256 / 256 % 256, curAddr / 256 % 256, curAddr % 256);
			printf("%d.%d.%d.%d", dstAddr / 256 / 256 / 256,
				dstAddr / 256 / 256 % 256, dstAddr / 256 % 256, dstAddr % 256);
			printf("\t%d\t", shortPaths[index].Dist);
			switch (shortPaths[index].Prev)
			{
			case -1:
				printf("%d.%d.%d.%d\n", curAddr / 256 / 256 / 256,
					curAddr / 256 / 256 % 256, curAddr / 256 % 256, curAddr % 256);
				break;
			default:
				printf("%d.%d.%d.%d\n", nextHop / 256 / 256 / 256,
					nextHop / 256 / 256 % 256, nextHop / 256 % 256, nextHop % 256);
				break;
			}
		}
		printf("\n\n");
	}
	int keyValue;
	printf("%sEnter r/R to restart, ESC or other to quit\n", InformationMsg);
	keyValue=getch();
	if(keyValue=='R'|| keyValue == 'r')
		goto START;
	return 0;
}

int InitOspf(Graph* G, unsigned int ipAddrs[]) {
	G->ArcNum = MaxArc;
	G->VexNum = MaxVex;
	for (int id = 1; id < MaxVex + 1; id++) {
		G->adjList[id].Data = ipAddrs[id - 1];
		G->adjList[id].FirstArc = NULL;
	}
}

int RandomWake(int vexNum)
{
	srand(time(NULL));
	int routerId = rand() % vexNum;
	return routerId;
}

int CheckWake(int num[], int count)
{
	int sum = 0;
	for (int i = 0; i < count; i++)
	{
		if (num[i] == -1)
			return 0;
	}
	return 1;
}


int AddRouter(Graph* G) {

	/*
	floodInfo[id] = InitFloodingInfo(ipAddr, 0x342770C005);
	ospfData* ospfData = InitOspfData(id,0x80000000,ipAddr,linkData[id-1][0], metrics[id-1][0]);
	ospfLSPacket* currentLSP= ospfData->LSP;
	if (floodInfo && ospfData && currentLSP) {
		for (int i = 0; i < maxPath; i++) {
			currentLSP->NextLSP = AddLSP(currentLSP, ipAddr, linkData[id-1][i], metrics[id - 1][i]);
			currentLSP = currentLSP->NextLSP;
		}
	}
	*/
	FILE* routerFileStream;
	char routerFileName[50];
	unsigned int linkData;
	unsigned int metrics;
	unsigned int id;
	int* wake = myNew(int, MaxVex);
	if (wake) {
		for (int i = 0; i < MaxVex; i++)
		{
			wake[i] = -1;
		}
	}
	
	while (1)
	{
		id = RandomWake(MaxVex);//random wake up
		if (CheckWake(wake, MaxVex))//complete wake up
			break;
		else
		{
			if (wake && wake[id] == 0)
			{
				continue;
			}
			else
			{
				printf("%sBooting router%d...\n",InformationMsg, id);
				sprintf(routerFileName, "Router%d.txt", id);
				routerFileStream = fopen(routerFileName, "r");
				if (routerFileStream == NULL)//判断文件是否为空
				{
					printf("%sFailed open %s,jump this file.\n", WarningMsg, routerFileName);
					wake[id] = 0;
				}
				else
				{
					int i, j;
					while (!feof(routerFileStream))
					{
						fscanf(routerFileStream, " %d %d", &linkData, &metrics);
						unsigned int ipAddr = 0xC0A80000 + id;
						linkData += 0xC0A80000;
						ArcNode* currentArc = (ArcNode*)malloc(sizeof(ArcNode));
						ArcNode* currentArc2 = (ArcNode*)malloc(sizeof(ArcNode));
						i = LocateVex(G, ipAddr);
						j = LocateVex(G, linkData);
						if (currentArc && currentArc2 && (i < j)) {

							currentArc->Adjvex = j;
							currentArc->Weight = metrics;
							currentArc->NextArc = G->adjList[id].FirstArc;
							G->adjList[id].FirstArc = currentArc;
							currentArc2->Adjvex = id;
							currentArc2->Weight = metrics;
							currentArc2->NextArc = G->adjList[j].FirstArc;
							G->adjList[j].FirstArc = currentArc2;
						}
					}
					wake[id] = 0;
					printf("%sComplete loading config of router%d\n", InformationMsg, id);
					fclose(routerFileStream);
				}
			}
		}
	}

}


void delay(int xms) {
	clock_t startTime = clock();
	while (clock() < startTime + xms);
}


/*
*
unsigned int linkData[MaxVex][MaxDepth] = {
	0xC0A80002,0xC0A80003,0xC0A80004,0,0,
	0xC0A80001,0xC0A80003,0xC0A80004,0,0,
	0xC0A80001,0xC0A80002,0xC0A80004,0xC0A80005,0,
	0xC0A80001,0xC0A80002,0xC0A80003,0xC0A80005,0xC0A80006,
	0xC0A80003,0xC0A80004,0xC0A80006,0,0,
	0xC0A80004,0xC0A80005,0,0,0
};
//*((unsigned int*)linkData + (id - 1) * MaxDepth + index)
unsigned int Metrics[MaxVex][MaxDepth] = {
	2,4,22,0,0,
	2,1,6,0,0,
	4,1,1,4,0,
	22,6,1,10,5,
	4,10,3,0,0,
	5,3,0,0,0
};
//*((unsigned int*)metrics + (id - 1) * MaxDepth + index)

	AddRouter(&linkGraph, 1, 0xC0A80001, 3, linkData, Metrics,floodInfo);
	AddRouter(&linkGraph, 2, 0xC0A80002, 3, linkData, Metrics, floodInfo);
	AddRouter(&linkGraph, 3, 0xC0A80003, 4, linkData, Metrics, floodInfo);
	AddRouter(&linkGraph, 4, 0xC0A80004, 5, linkData, Metrics, floodInfo);
	AddRouter(&linkGraph, 5, 0xC0A80005, 3, linkData, Metrics, floodInfo);
	AddRouter(&linkGraph, 6, 0xC0A80006, 2, linkData, Metrics, floodInfo);


*/

/*

#define MAXNUM 6
#define INF 10000
typedef struct {
	int points[MAXNUM];
	int matrix[MAXNUM][MAXNUM];
	int vertex_num;
	int edge_num;
} GRAPH;

void init_matrix(GRAPH* graph, int vertex_num, int edge_num);
void add_edge(GRAPH* graph, int point, int con, int weight);
void print_matrix(GRAPH* graph);
int AddPoint(GRAPH* graph, int dist[], int visited[]);
int find_parent(GRAPH* graph, int parent[], int i, int SoursePoint);
void dijkstraMatrix(GRAPH* graph, int VertexNum, int EdgeNum, int SoursePoint);




	GRAPH graph;
	int vertex_num = MAXNUM;
	int edge_num = 10;
	int sourse_point;



	sourse_point--;
	init_matrix(&graph, vertex_num, edge_num);//初始化通信网

	add_edge(&graph, 1, 2, 2);
	add_edge(&graph, 1, 3, 4);
	add_edge(&graph, 1, 4, 22);
	add_edge(&graph, 2, 3, 1);
	add_edge(&graph, 2, 4, 6);
	add_edge(&graph, 3, 4, 1);
	add_edge(&graph, 3, 5, 4);
	add_edge(&graph, 4, 5, 10);
	add_edge(&graph, 4, 6, 5);
	add_edge(&graph, 5, 6, 3);

	print_matrix(&graph); //打印矩阵

	dijkstraMatrix(&graph, vertex_num, edge_num, sourse_point);


void init_matrix(GRAPH* graph, int vertexNum, int edgeNum) {
	graph->vertex_num = vertexNum;
	graph->edge_num = edgeNum;
	for (int i = 0; i < vertexNum; i++) {
		graph->points[i] = i + 1;
		for (int j = 0; j < vertexNum; j++) {
			if (i == j) graph->matrix[i][j] = 0;
			else graph->matrix[i][j] = INF;
		}
	}
}

void add_edge(GRAPH* graph, int row, int con, int weight) {
	graph->matrix[row - 1][con - 1] = weight;//顶点是123456，传进数组时要减一
	graph->matrix[con - 1][row - 1] = weight;
}

void print_matrix(GRAPH* graph) {
	for (int i = 0; i < graph->vertex_num; i++) {
		for (int j = 0; j < graph->vertex_num; j++) {
			printf("%6d", graph->matrix[i][j]);
		}
		printf("\n");
	}
}

int AddPoint(GRAPH* graph, int dist[], int visited[]) {
	int min = INF;
	int PointIndex;
	for (int i = 0; i < graph->vertex_num; i++) {
		if (visited[i] == 0 && dist[i] <= min) {
			min = dist[i];
			PointIndex = i;
		}
	}
	return PointIndex;
}

int find_parent(GRAPH* graph, int parent[], int i, int SoursePoint) {
	if (parent[i] == SoursePoint) return i;
	else {
		i = parent[i];
		find_parent(graph, parent, i, SoursePoint);
	}
}

void dijkstraMatrix(GRAPH* graph, int VertexNum, int EdgeNum, int SoursePoint) {
	int *dist;
	int *visited;
	int *parent;
	int *nextHop;
	dist = (int*)malloc(VertexNum * sizeof(int));
	visited = (int*)malloc(VertexNum * sizeof(int));
	parent = (int*)malloc(VertexNum * sizeof(int));
	nextHop= (int*)malloc(VertexNum * sizeof(int));
	int PointIndex;

	for (int i = 0; i < VertexNum; i++) {
		dist[i] = INF;
		visited[i] = 0;
		parent[i] = -1;
	}
	dist[SoursePoint] = 0;
	parent[SoursePoint] = SoursePoint;

	for (int i = 0; i < VertexNum; i++) {
		PointIndex = AddPoint(graph, dist, visited);
		visited[PointIndex] = 1;

		for (int j = 0; j < VertexNum; j++) {
			if (!visited[j] && graph->matrix[PointIndex][j] && dist[PointIndex] != INF
				&& dist[PointIndex] + graph->matrix[PointIndex][j] < dist[j]) {
				dist[j] = dist[PointIndex] + graph->matrix[PointIndex][j];
				parent[j] = PointIndex;
			}
		}
	}
	for (int i = 0; i < VertexNum; i++) {
		nextHop[i] = find_parent(graph, parent, i, SoursePoint);
	}
	printf("Dest NextHop\n");
	for (int i = 0; i < VertexNum; i++) {
		printf("%4d   %4d\n", i + 1, nextHop[i] + 1);
	}
	free(dist);
	free(visited);
	free(parent);
	free(nextHop);
}

*/