#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "solver.h"

struct Queue {
	int size;
	int len;
	int *queue;
};

struct Queue queue_new(int size)
{
	struct Queue q;
	q.size = size;
	q.len = 0;
	q.queue = malloc(sizeof(int) * size);
	return q;
}

void queue_append(struct Queue *q, int value)
{
	if (q->len + 1 >= q->size) {
		q->size += 128;
		q->queue = realloc(q->queue, sizeof(int) * q->size);
	}

	q->queue[q->len] = value;
	q->len++;
}

void conditional_append(struct Queue *q, struct Maze *maze, int pos, char flag, int mov_x, int mov_y)
{
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

void maze_solve(struct Maze *maze)
{
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

void solution_bin(struct Maze *maze)
{
	FILE *maze_file = fopen(maze->path, "r+");
	if (maze_file == NULL) {
		printf("Could not open maze file to write solution: %s\n", maze->path);
		exit(1);
	}

	int counter;
	fseek(maze_file, 29, SEEK_SET);
	fread(&counter, 4, 1, maze_file);
	int sol_offset = 40 + counter * 3;
	fwrite(&sol_offset, 4, 1, maze_file);
	fseek(maze_file, sol_offset + 8, SEEK_SET);
	
	int steps = 0;

	int current = maze->entry;
	int x = current % maze->width;
	int y = current / maze->width;
	char rotation = *maze_get(maze, x, y) & 15;
	char distance = 0;

	char tile;
	while (current != maze->exit) {
		tile = *maze_get(maze, x, y) & 15;

		if (rotation == tile) {
			distance++;
		}
		else {
			distance--;
                        char rotation_char = ' ';
                        switch (rotation) {
                        case SRC_UP:
                                rotation_char = 'N';
                                break;
                        case SRC_RIGHT:
                                rotation_char = 'E';
                                break;
                        case SRC_DOWN:
                                rotation_char = 'S';
                                break;
                        case SRC_LEFT:
                                rotation_char = 'W';
                                break;
                        }
			fwrite(&rotation_char, 1, 1, maze_file);
			fwrite(&distance, 1, 1, maze_file);
			rotation = tile;
			distance = 1;
			steps++;
		}

		switch (tile) {
                case SRC_UP:
                        y--;
                        break;
                case SRC_RIGHT:
                        x++;
                        break;
                case SRC_DOWN:
                        y++;
                        break;
                case SRC_LEFT:
                        x--;
                        break;
		}
		current = y * maze->width + x;

	}
	distance--;
	fwrite(&rotation, 1, 1, maze_file);
	fwrite(&distance, 1, 1, maze_file);
	steps++;
	
	int identifier = 0x52524243;

	fseek(maze_file, sol_offset, SEEK_SET);
	fwrite(&identifier, 4, 1, maze_file);
	fwrite(&steps, 4, 1, maze_file);
	fclose(maze_file);
}

void rotate(char *rotation, char target)
{
	if (*rotation == SRC_UP && target == SRC_LEFT) {
		printf("TURN LEFT\n");
		*rotation = SRC_LEFT;
	} else if (*rotation == SRC_LEFT && target == SRC_UP) {
		printf("TURN RIGHT\n");
		*rotation = SRC_UP;
	} else if (*rotation << 1 == target) {
		printf("TURN LEFT\n");
		*rotation <<= 1;
	} else if (*rotation >> 1 == target) {
		printf("TURN RIGHT\n");
		*rotation >>= 1;
	}
}

void solution_txt(struct Maze *maze)
{
	int current = maze->entry;
	int x = current % maze->width;
	int y = current / maze->width;
	char rotation = *maze_get(maze, x, y) & 15;
	char distance = 0;

	char tile;
	printf("START\n");
	while (current != maze->exit) {
		tile = *maze_get(maze, x, y) & 15;
		if ((tile) == rotation) {
			distance++;
		} else {
			printf("FORWARD %i\n", distance);
			distance = 1;
			rotate(&rotation, tile);
		}

		switch (rotation) {
                case SRC_UP:
                        y--;
                        break;
                case SRC_RIGHT:
                        x++;
                        break;
                case SRC_DOWN:
                        y++;
                        break;
                case SRC_LEFT:
                        x--;
                        break;
		}
		current = y * maze->width + x;
	
	}
	printf("FORWARD %i\n", distance);
	printf("STOP\n");
}

void maze_solution(struct Maze *maze)
{
	switch (maze->type) {
        case unknown:
                printf("Invalid maze type\n");
                exit(1);
                break;
        case bin:
                solution_bin(maze);
                break;
        case txt:
                solution_txt(maze);
                break;
	}
}
