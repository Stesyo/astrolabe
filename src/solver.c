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
		q->queue = realloc(q->queue, sizeof(int) * q->size);
	}

	q->queue[q->len] = value;
	q->len++;
}

void conditional_append(struct Queue *q, struct Maze *maze, int pos, char flag, int mov_x, int mov_y) {
	int x = pos % maze->width + mov_x;
	int y = pos / maze->width + mov_y;

	if (0 > x || 0 > y || x >= maze->width || y >= maze->height) {
		return;
	}
	
	char *tile = maze_get(maze, x, y);
	if (*tile & (VISITED | WALL)) {
		return;
	}
	(*tile) |= flag | VISITED;

	queue_append(q, y * maze->width + x);
}

void maze_solve(struct Maze *maze) {
	struct Queue q_now = queue_new(128);
	struct Queue q_next = queue_new(128);
	queue_append(&q_now, maze->exit);

	struct Queue tmp;
	while (q_now.len > 0) {

		for (int i = 0; q_now.len > i; i++) {
			int pos = q_now.queue[i];
			conditional_append(&q_next, maze, pos, SRC_DOWN, 0, -1);
			conditional_append(&q_next, maze, pos, SRC_RIGHT, -1, 0);
			conditional_append(&q_next, maze, pos, SRC_LEFT, 1, 0);
			conditional_append(&q_next, maze, pos, SRC_UP, 0, 1);
		}
		
		tmp = q_now;
		q_now = q_next;
		q_next = tmp;

		q_next.len = 0;
	}
	free(q_now.queue);
	free(q_next.queue);
}
