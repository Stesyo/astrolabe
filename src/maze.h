#ifndef MAZE_H

static const char TILE = -128;
static const char WALL = 64;
static const char VISITED = 32;
static const char SRC_UP = 8;
static const char SRC_RIGHT = 4;
static const char SRC_DOWN = 2;
static const char SRC_LEFT = 1;

static const int CHUNK_SIZE = 64;
static const int BELT_SIZE = 16;

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

struct Chunk {
	int index;
	int width;
	int height;
	char **field;
};

struct Maze maze_load(char *maze_path);

void maze_solution(struct Maze *maze);

void maze_free(struct Maze *maze);

char *maze_get(struct Maze *maze, int x, int y);

#endif // !MAZE_H
