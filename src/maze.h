#ifndef MAZE_H
#define MAZE_H

#include "chunk.h"

static const char TILE = -128;
static const char WALL = 64;
static const char VISITED = 32;
static const char SRC_UP = 8;
static const char SRC_RIGHT = 4;
static const char SRC_DOWN = 2;
static const char SRC_LEFT = 1;


enum file_type {
	unknown,
	bin,
	txt
};

struct Maze {
	int width;
        int height;
        int entry;
        int exit;
	enum file_type type;
	char *path;
	struct Chunk *chunk_belt;
};

struct Maze maze_load(char *maze_path);

void maze_solution(struct Maze *maze);

void maze_free(struct Maze *maze);

char *maze_get(struct Maze *maze, int x, int y);

#endif // !MAZE_H
