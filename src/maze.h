#ifndef MAZE_H
#define MAZE_H

#include "chunk.h"

#define TILE -128
#define WALL 64
#define VISITED 32
#define SRC_UP 8
#define SRC_RIGHT 4
#define SRC_DOWN 2
#define SRC_LEFT 1


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

void maze_free(struct Maze *maze);

char *maze_get(struct Maze *maze, int x, int y);

#endif // !MAZE_H
