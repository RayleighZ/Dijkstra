#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node node;
typedef struct Edge edge;
typedef struct NodeArrayWrapper nodeArrayWrapper;
typedef struct PathLink pathLink;

const int INT_MAX = 0x11111111;

struct Node {
    edge *edgeArray;
    int edgeCount;
    int id;
    int flag;
};
struct Edge {
    int length;
    node *lightSide;
    node *darkSide;
};

struct NodeArrayWrapper {
    node *nodeArray;
    int size;
};

//链路的path，采取链表的存储方式，一个节点代表一个路径，char数组表示的是节点的index
//为啥要用char？1 空间小于int，2 有库函数可以用
struct PathLink {
    char *path;
    pathLink *next;
};

edge *edgeBuilder(node *lightNode, node *darkNode, int dis) {
    edge *_edge = (edge *) malloc(sizeof(edge));
    _edge->length = dis;
    _edge->darkSide = darkNode;
    _edge->lightSide = lightNode;
    return _edge;
}

nodeArrayWrapper *initNodeArray() {
    FILE *mapFile = fopen("D:\\C_Lab_Data\\lab2\\Map.txt", "r");
    if (mapFile == NULL) {
        printf("Error: file not find\n");
    }
    //读取mapFile的首位（节点个数）
    char nodeCount_C[16];
    if (fscanf(mapFile, "%s", nodeCount_C) != 1) {
        printf("Error: file format isn't as wished");
        exit(0);
    }
    nodeArrayWrapper *naw = (nodeArrayWrapper *) malloc(sizeof(nodeArrayWrapper));
    naw->size = atoi(nodeCount_C);
    naw->nodeArray = (node *) malloc(sizeof(node) * naw->size);
    //分配edgeArray的内存空间
    for (int i = 0; i < naw->size; ++i) {
        naw->nodeArray[i].edgeArray = (edge *) malloc(sizeof(edge) * naw->size);
        naw->nodeArray[i].edgeCount = 0;
    }
    //接下来循环生成naw数组
    char node1Id_C[16];
    char node2Id_C[16];
    char distance_C[16];
    while (fscanf(mapFile, "%s %s %s", node1Id_C, node2Id_C, distance_C) == 3) {
        int node1Id = atoi(node1Id_C);
        int node2Id = atoi(node2Id_C);
        int distance = atoi(distance_C);
        node node1 = naw->nodeArray[node1Id];
        node node2 = naw->nodeArray[node2Id];
        node1.id = node1Id;
        node2.id = node2Id;
        node1.edgeArray[node1.edgeCount] = *edgeBuilder(&node1, &node2, distance);
        node2.edgeArray[node2.edgeCount] = *edgeBuilder(&node2, &node1, distance);

        node1.edgeCount++;
        node2.edgeCount++;
    }
    return naw;
}

int isAllOne(const int *array, int size) {
    for (int i = 0; i < size; ++i) {
        if (!array[i]) {
            return 0;
        }
    }
    return 1;
}

void dijkstra(nodeArrayWrapper *naw, int startNodeIndex) {

    //初始化最短路数组
    pathLink *minPaths = (pathLink *)malloc(sizeof(pathLink) * naw->size);
    for (int i = 0; i < naw->size; ++i) {
        minPaths->path = (char *)malloc(sizeof(char)*naw->size);
    }
    minPaths[startNodeIndex].path[0] = startNodeIndex + '0';
    minPaths[startNodeIndex].path[1] = 0;

    //声明计算用数组
    int length[naw->size];//缓存最大长度的数组
    length[0] = 0;//到自身的距离为0
    int blueJudgeArray[naw->size];//缓存是否加入蓝色节点队列的数组
    for (int i = 0; i < naw->size; ++i) {
        blueJudgeArray[i] = 0;
    }
    blueJudgeArray[startNodeIndex] = 1;

    node *nodeArray = naw->nodeArray;
    for (int i = 1; i < naw->size; ++i) {
        length[i] = INT_MAX;
    }

    node currentBlueNode = nodeArray[startNodeIndex];

    while (!isAllOne(blueJudgeArray, naw->size)) {
        for (int i = 0; i < currentBlueNode.edgeCount; ++i) {
            edge curEdge = currentBlueNode.edgeArray[i];
            node *darkSide = currentBlueNode.edgeArray[i].darkSide;
            int refreshLength = curEdge.length + length[currentBlueNode.id];
            if (refreshLength < length[darkSide->id]) {
                //认定为可刷新最短路径
                length[darkSide->id] = refreshLength;
                //拼接最短路
                char darkSideMinPath[naw->size];
                strcpy(darkSideMinPath, minPaths[currentBlueNode.id].path);//先把blueNode上的最短路径复制过来
                char curIndex[2];
                curIndex[1] = 0;
                curIndex[0] = darkSide->id + '0';
                strcat(darkSideMinPath, curIndex);
            }
        }
    }
}

int main() {
    nodeArrayWrapper naw = *initNodeArray();
    return 0;
}