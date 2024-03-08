#ifndef MAZE_H

static const int CHUNK_SIZE = 64;
static const int BELT_SIZE = 16;

struct Maze {
	int width;
        int height;
        int entry;
        int exit;
	struct Chunk *chunk_belt;
};

struct Chunk {
	int index;
	int width;
	int height;
	char **field;
};

struct Maze maze_load(char *maze_path);

void maze_free(struct Maze *maze);

char *maze_get(struct Maze *maze, int x, int y);

#endif /* ifndef MAZE_H */
