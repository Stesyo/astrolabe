#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include"maze.h"

/*TO FIX
- TRACKING BACK
- VISITED MEMORY
*/

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
    // Define the directions: up, down, left, right
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    // Create a visited array
    bool visited[MAX_SIZE][MAX_SIZE] = {false};

    // Create a queue for BFS
    Queue* queue = createQueue();

    int entryX = maze.entry % maze.width;
    int entryY = maze.entry / maze.width;

    // Mark the entry point as visited and enqueue it
    Position start = {entryX, entryY};
    visited[entryX][entryY] = true;
    enqueue(queue, start);

    int exitX = maze.exit % maze.width;
    int exitY = maze.exit / maze.width;

    // Perform BFS
    while (!isEmpty(queue)) {
        Position current = dequeue(queue);

        // Check if exit is reached
        if (current.x == exitX && current.y == exitY) {
            break;
        }

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (isValid(newX, newY, width, height) && *maze_get(&maze,newX,newY) == ' ' && !visited[newX][newY]) {
                visited[newX][newY] = true;
                enqueue(queue, (Position){newX, newY});
            }
        }
    }
    /*
    printf("Checked");
    // Trace back the path and save it to the file
    Position current = {exitX, exitY};
    int count = 0;
   while(current.x != entryX || current.y != entryY){
        //visited[current.x][current.y] = false;
        if(isValid(current.x + 1, current.y, width, height) && visited[current.x + 1][current.y]) {
            printf("LEFT\n");
            current.x++;
        } else if (isValid(current.x, current.y + 1, width, height) && visited[current.x][current.y + 1]) {
            printf("UP\n");
            current.y++;
        } else if (isValid(current.x - 1, current.y, width, height) && visited[current.x - 1][current.y]) {
            printf("RIGHT\n");
            current.x--;
        } else if (isValid(current.x, current.y - 1, width, height) && visited[current.x][current.y - 1]) {
            printf("DOWN\n");
            current.y--;
        }
        count ++;
   }

   printf("%i\n",count) ;*/

    // Free allocated memory
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}

