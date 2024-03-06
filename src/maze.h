#ifndef MAZE_H

static const int CHUNK_COUNT = 16;
static const int CHUNK_SIZE = 64;

struct Maze {
	int width;
        int height;
        int entry;
        int exit;
	struct Chunk *chunk_belt;
};

struct Chunk {
	int index;
	int x;
	int y;

	int width;
	int height;
	char **field;
};

struct Maze maze_load(char *maze_path);

void maze_free(struct Maze maze);

#endif /* ifndef MAZE_H */
