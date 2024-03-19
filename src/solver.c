#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "solver.h"

struct Queue {
	int size;
	int len;
	int *queue;
};

struct Queue queue_new(int size) {
	struct Queue q;
	q.size = size;
	q.len = 0;
	q.queue = malloc(sizeof(int) * size);
	return q;
}

void queue_append(struct Queue *q, int value) {
	if (q->len + 1 >= q->size) {
		q->size += 128;
		q->queue = realloc(q->queue, sizeof(int) *q->size);
	}

	q->queue[q->len] = value;
	q->len++;
}

void conditional_append(struct Maze *maze, struct Queue *q, int pos, int src, int mov_x, int mov_y) {
	int x = pos % maze->width + mov_x;
	int y = pos / maze->width + mov_y;

	if (0 > x || 0 > y || x > maze->width || y > maze->height) {
		return;
	}

	char *tile = maze_get(maze, x, y);
	if (*tile & (VISITED | WALL)) {
		return;
	}
	(*tile) += src + VISITED;

	queue_append(q, y * maze->width + x);
}

void maze_solve(struct Maze *maze) {
	struct Queue q = queue_new(1024);
	queue_append(&q, maze->entry);

	struct Queue new_q;

	while (q.len > 0) {
		printf("%i\n", q.len);
		new_q = queue_new(1024);
		for (int i = 0; q.len > i; i++) {
			int pos = q.queue[i];
			conditional_append(maze, &new_q, pos, SRC_DOWN, 0, -1);
			conditional_append(maze, &new_q, pos, SRC_RIGHT, -1, 0);
			conditional_append(maze, &new_q, pos, SRC_LEFT, 1, 0);
			conditional_append(maze, &new_q, pos, SRC_UP, 0, 1);
		}
		free(q.queue);
		q = new_q;
	}
	free(q.queue);
}
