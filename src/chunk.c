#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "chunk.h"

int int_digits(int value) {
	if (value == 0) {
                return 1;
        }
	int digits = 0;
	while (value != 0) { 
		value /= 10; 
		digits++; 
	}
	return digits;
}

struct Chunk chunk_new(int width, int height) {
	struct Chunk chunk;
	chunk.width = width;
	chunk.height = height;
	chunk.field = malloc(sizeof(char *) * height);
	for (int i = 0; height > i; i++) {
		chunk.field[i] = malloc(sizeof(char) * width);
	}
	return chunk;
}

struct Chunk chunk_load(int index) {
	char chunk_path[19] = "temp/chunk_000.txt";
	sprintf(chunk_path + 14 - int_digits(index), "%i", index);

	FILE *chunk_file = fopen(chunk_path, "r");
	if (chunk_file == NULL) {
		printf("Could not oped chunk file: %s\n", chunk_path);
		exit(1);
	}

	size_t buffer_len = sizeof(char) * CHUNK_SIZE + 2;
	char *buffer = malloc(buffer_len);

	getline(&buffer, &buffer_len, chunk_file);
	
	int width;
	int height;
	sscanf(buffer, "%d %d", &width, &height);

	struct Chunk chunk;
	chunk.index = index;
	chunk.width = width;
	chunk.height = height;
	chunk.field = malloc(sizeof(char *) * height);
	if (chunk.field == NULL) {
		printf("Could not allocate memory for chunk.field\n");
		exit(1);
	}

	for (int i = 0; height > i; i++) {
		chunk.field[i] = malloc(sizeof(char) * width);
		int len = getline(&buffer, &buffer_len, chunk_file);
		if (len == -1) {
			printf("Chunk file ended abruptly: [%i:%i]\n", index, i);
			exit(1);
		}
		memcpy(chunk.field[i], buffer, sizeof(char) * width);
	}
	fclose(chunk_file);
	free(buffer);

	return chunk;
}

void chunk_free(struct Chunk *chunk) {
	if (chunk->field == NULL) {return;}
	for (int i = 0; chunk->height > i; i++) {
		free(chunk->field[i]);
	}
	free(chunk->field);
}

void chunk_write(struct Chunk *chunk) {
	if (chunk->index == -1) {
		return;
	}
	char chunk_path[19] = "temp/chunk_000.txt";
	sprintf(chunk_path + 14 - int_digits(chunk->index), "%i", chunk->index);

	FILE *chunk_file = fopen(chunk_path, "w");
	if (chunk_file == NULL) {
		printf("Could not open chunk file in write mode, chunk: %i\n", chunk->index);
		exit(1);
	}

	fprintf(chunk_file, "%i %i\n", chunk->width, chunk->height);
	for (int i = 0; chunk->height > i; i++) {
		fwrite(chunk->field[i], sizeof(char), chunk->width, chunk_file);
		fwrite("\n", sizeof(char), 1, chunk_file);
	}
	fclose(chunk_file);
}


struct Belt belt_new(int width, int height) {
	struct Belt belt;

	int length = (width + CHUNK_SIZE - 1) / CHUNK_SIZE;

	belt.length = length;
	belt.width = width;
	belt.height = height;
	belt.chunks = malloc(sizeof(struct Chunk) * length);
	for (int i = 0; length > i; i++) {
		int chunk_width = (width - i * CHUNK_SIZE) >= CHUNK_SIZE ? CHUNK_SIZE : width % CHUNK_SIZE;
		belt.chunks[i] = chunk_new(chunk_width, CHUNK_SIZE);
		belt.chunks[i].index = i;
	}

	return belt;
}

void belt_free(struct Belt *belt) {
	for (int i = 0; belt->length > i; i++) {
		belt->chunks[i].height = belt->height;
		chunk_free(&belt->chunks[i]);
	}
	free(belt->chunks);
}

void belt_write(struct Belt *belt, int height) {
	for (int i = 0; belt->length > i; i++) {
		belt->chunks[i].height = height;
		chunk_write(&belt->chunks[i]);
		belt->chunks[i].index += belt->length;
	}

}

void belt_set(struct Belt *belt, int x, int y, int value) {
	belt->chunks[x / CHUNK_SIZE].field[y][x % CHUNK_SIZE] = value;
}
