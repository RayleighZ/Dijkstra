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

void showPath(pathLink *path) {
    pathLink *cursor = path;
    while (cursor != NULL) {
        unsigned int length = strlen(cursor->path);
        for (int i = 0; i < length - 1; ++i) {
            printf("%c --> ", cursor->path[i] + 1);
        }
        printf("%c\n", cursor->path[length - 1] + 1);
        cursor = cursor->next;
    }
}

edge *edgeBuilder(node *lightNode, node *darkNode, int dis) {
    edge *_edge = (edge *) malloc(sizeof(edge));
    _edge->length = dis;
    _edge->darkSide = darkNode;
    _edge->lightSide = lightNode;
    return _edge;
}

//整数转型为字符串
char *int2String(int mark) {
    int length = 0;
    int cMark = mark;
    do {
        cMark /= 10;
        length++;
    } while (cMark != 0);
    char *string = (char *) malloc(sizeof(char) * length);
    string[length] = 0;
    for (int i = length - 1; i >= 0; --i) {
        string[i] = mark % 10 + '0';
        mark /= 10;
    }
    return string;
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
        naw->nodeArray[node1Id - 1].id = node1Id - 1;
        naw->nodeArray[node2Id - 1].id = node2Id - 1;
        naw->nodeArray[node1Id - 1].flag = 0;
        naw->nodeArray[node2Id - 1].flag = 0;
        naw->nodeArray[node1Id - 1].edgeArray[naw->nodeArray[node1Id - 1].edgeCount] = *edgeBuilder(
                &naw->nodeArray[node1Id - 1], &naw->nodeArray[node2Id - 1], distance);
        naw->nodeArray[node2Id - 1].edgeArray[naw->nodeArray[node2Id - 1].edgeCount] = *edgeBuilder(
                &naw->nodeArray[node2Id - 1], &naw->nodeArray[node1Id - 1], distance);

        naw->nodeArray[node1Id - 1].edgeCount++;
        naw->nodeArray[node2Id - 1].edgeCount++;
    }
    return naw;
}


pathLink *dijkstra(nodeArrayWrapper *naw, int startNodeIndex) {

    //初始化最短路数组
    pathLink *minPaths = (pathLink *) malloc(sizeof(pathLink) * naw->size);
    for (int i = 0; i < naw->size; ++i) {
        minPaths[i].path = (char *) malloc(sizeof(char) * naw->size);
        minPaths[i].next = NULL;
    }
    minPaths[startNodeIndex].path[0] = startNodeIndex + '0';
    minPaths[startNodeIndex].path[1] = 0;

    //声明计算用数组
    int length[naw->size];//缓存最大长度的数组
    for (int i = 1; i < naw->size; ++i) {
        length[i] = INT_MAX;
    }
    length[startNodeIndex] = 0;//到自身的距离为0

    node *nodeArray = naw->nodeArray;
    nodeArray[startNodeIndex].flag = 1;
    node currentBlueNode = nodeArray[startNodeIndex];
    int blueNodeCount = 0;
    while (blueNodeCount < naw->size) {
        //首先根据cbn刷新到达起始点的最短路径
        for (int i = 0; i < currentBlueNode.edgeCount; ++i) {
            node *darkSide = currentBlueNode.edgeArray[i].darkSide;
            if (darkSide->flag){//如果已经确定了到达对应节点的最短路径，就直接跳过
                continue;
            }
            //判断新节点是否可以更新最短路径
            int curLength = currentBlueNode.edgeArray[i].length + length[currentBlueNode.id];
            if (length[darkSide->id] >= curLength) {

                //形成新的最短路径
                char *path = (char *)malloc(sizeof(char) * naw->size);
                strcpy(path, minPaths[currentBlueNode.id].path);
                strcat(path, int2String(darkSide->id));//将新的最后一位（darkSide添加进去）
                if (length[darkSide->id] == curLength) {
                    //连接minPaths darkSide位置的链表
                    pathLink * pathLink_newMin = (pathLink *)malloc(sizeof(pathLink));
                    pathLink_newMin->next = NULL;
                    pathLink_newMin->path = path;
                    minPaths[darkSide->id].next = pathLink_newMin;
                } else {//不需要拼接，直接强行刷新
                    //刷新最短路径长度
                    length[darkSide->id] = curLength;
                    minPaths[darkSide->id].path = path;
                    //释放链表后续指针所在地址
                    pathLink * cursor = minPaths[darkSide->id].next;
                    pathLink * freePoint;
                    while (cursor != NULL){
                        freePoint = cursor;
                        cursor = cursor->next;
                        free(freePoint);
                    }
                    minPaths[darkSide->id].next = NULL;//保证之前的后续路径被丢弃掉
                }
            }
        }
        int minToStartLength = INT_MAX;
        int minNodeId;
        for (int i = 0; i < naw->size; ++i) {
            if (length[i] < minToStartLength && !nodeArray[i].flag){
                minNodeId = i;
                minToStartLength = length[i];
            }
        }
        //再根据当前最短节点刷新cbn
        currentBlueNode = nodeArray[minNodeId];
        //判定为flag为1
        nodeArray[minNodeId].flag = 1;
        blueNodeCount++;
    }
    return minPaths;
}

int main() {
    nodeArrayWrapper *naw = initNodeArray();
    pathLink *minPaths = dijkstra(naw, 3);
    for (int i = 0; i < naw->size; ++i) {
        showPath(&minPaths[i]);
    }
    return 0;
}