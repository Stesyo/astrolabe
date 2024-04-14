#ifndef CHUNK_H
#define CHUNK_H

static const int CHUNK_SIZE = 64;
static const int BELT_SIZE = 16;

struct Chunk {
	int index;
	int width;
	int height;
	char **field;
};

struct Belt {
	int width;
	int height;
	int length;
	struct Chunk *chunks;
};

struct Chunk chunk_new(int width, int height);

struct Chunk chunk_load(int index);

void chunk_free(struct Chunk *chunk);

void chunk_write(struct Chunk *chunk);


struct Belt belt_new(int width, int height);

void belt_free(struct Belt *belt);

void belt_write(struct Belt *belt, int height);

void belt_set(struct Belt *belt, int x, int y, int value);

#endif // !CHUNK_H
