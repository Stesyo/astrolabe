#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include"maze.h"

#define MAX_QUEUE_SIZE 512
#define MAX_SIZE 1024

typedef struct {
    int x;
    int y;
} Position;

typedef struct QueueNode {
    Position data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode *front, *rear;
    unsigned int size;
} Queue;

QueueNode* createNode(Position data) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

bool isEmpty(Queue* queue) {
    return queue->size == 0;
}

void enqueue(Queue* queue, Position data) {
    QueueNode* newNode = createNode(data);
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

Position dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        Position empty = {-1, -1};
        return empty;
    }
    QueueNode* temp = queue->front;
    Position data = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    queue->size--;
    return data;
}

bool isValid(int x, int y, int width, int height) {
    return (x >= 0 && x < height && y >= 0 && y < width);
}
void findShortestPath(struct Maze maze, int width, int height, FILE* file) {
    // kierunki
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    char rot[] = {'E','W','S','N'};

    Queue* queue = createQueue();

    int entryX = maze.entry % maze.width;
    int entryY = maze.entry / maze.width;

    Position start = {entryX, entryY};
    enqueue(queue, start);

    int exitX = maze.exit % maze.width;
    int exitY = maze.exit / maze.width;

    // BFS
    while (!isEmpty(queue)) {
        Position current = dequeue(queue);

        if (current.x == exitX && current.y == exitY) {
            break;
        }

        for (int i = 0; i < 4; i++) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (isValid(newX, newY, width, height) && *maze_get(&maze,newX,newY) == ' ') {
                enqueue(queue, (Position){newX, newY});
                // skąd się tu dostał
                *maze_get(&maze,newX,newY) = rot[i];
            }
        }
    }
    
    //Zaznaczenie najkrótszej ścieżki
    int curX = exitX;
    int curY = exitY;
    while(curX != entryX || curY != entryY){
        switch (*maze_get(&maze,curX,curY))
        {
        case 'N':
            *maze_get(&maze,curX,curY) = 'F';
            curY --;
            break;
        case 'S':
            *maze_get(&maze,curX,curY) = 'F';
            curY ++;
            break;
        case 'E':
            *maze_get(&maze,curX,curY) = 'F';
            curX ++;
            break;
        case 'W':
            *maze_get(&maze,curX,curY) = 'F';
            curX --;
            break;
        default:
            break;
        }
    }
    curX = entryX;
    curY = entryY;
    int count = 0;

    //Wypisanie listy kroków
    
    fprintf(file,"START\n");
    while(curX != exitX || curY != exitY){
        if(isValid(curX + 1, curY, width, height) && *maze_get(&maze,curX+1,curY) == 'F'){
            while(isValid(curX + 1, curY, width, height) && *maze_get(&maze,curX+1,curY) == 'F'){
            curX ++;
            count++;
            *maze_get(&maze,curX,curY) = 'c';}
            fprintf(file, "RIGHT %i\n", count);
            count = 0;
        }
        if(isValid(curX - 1, curY, width, height) && *maze_get(&maze,curX -1,curY) == 'F'){
            while(isValid(curX - 1, curY, width, height) && *maze_get(&maze,curX-1,curY) == 'F'){
            curX --;
            count++;
            *maze_get(&maze,curX,curY) = 'c';}
            fprintf(file, "LEFT %i\n", count);
            count = 0;
        }
        if(isValid(curX, curY + 1, width, height) && *maze_get(&maze,curX,curY + 1) == 'F'){
            while(isValid(curX, curY + 1, width, height) && *maze_get(&maze,curX,curY + 1) == 'F'){
            curY++;
            count++;
            *maze_get(&maze,curX ,curY ) = 'c';}
            fprintf(file, "DOWN %i\n", count);
            count = 0;
        }
        if(isValid(curX, curY - 1, width, height) && *maze_get(&maze,curX,curY - 1) == 'F'){
            while(isValid(curX, curY - 1, width, height) && *maze_get(&maze,curX,curY - 1) == 'F'){
            curY--;
            count++;
            *maze_get(&maze,curX ,curY ) = 'c';}
            fprintf(file, "UP %i\n", count);
            count = 0;
        }
    }
    
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}