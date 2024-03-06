#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>

#include "maze.h"

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

void chunk_write(struct Chunk *chunk) {
	char chunk_path[19] = "temp/chunk_000.txt";
	sprintf(chunk_path + 14 - int_digits(chunk->index), "%i", chunk->index);

	FILE *chunk_file = fopen(chunk_path, "w");
	if (chunk_file == NULL) {
		printf("Could not open chunk file in write mode, chunk: %i\n", chunk->index);
		exit(1);
	}

	for (int i = 0; chunk->height > i; i++) {
		fwrite(chunk->field[i], sizeof(char), chunk->width, chunk_file);
		fwrite("\n", sizeof(char), 1, chunk_file);
	}
	fclose(chunk_file);
}

void chunk_free(struct Chunk *chunk) {
	for (int i = 0; chunk->height > i; i++) {
		free(chunk->field[i]);
	}
	free(chunk->field);
}

struct Chunk chunk_new(int width, int height) {
	struct Chunk chunk;
	chunk.field = malloc(sizeof(char *) * height);
	for (int i = 0; height > i; i++) {
		chunk.field[i] = malloc(sizeof(char) * width);
	}
	chunk.width = width;
	chunk.height = height;
	return chunk;
}

struct Chunk chunk_load(int index) {
	return chunk_new(1, 1);
}

struct Maze maze_load(char *maze_path) {
	FILE *maze_file = fopen(maze_path, "r");
	if (maze_file == NULL) {
		printf("Could not open maze file: %s\n", maze_path);
		exit(1);
	}

        mkdir("temp", 0755);
	struct Maze maze;

	maze.width = 0;
	maze.height = 0;
	maze.exit = -1;
	maze.exit = -1;

	while (fgetc(maze_file) != '\n') {
		maze.width += 1;
	}
	rewind(maze_file);

	int chunks_len = (maze.width + CHUNK_SIZE - 1) / CHUNK_SIZE;
	struct Chunk *chunks = malloc(sizeof(struct Chunk) * chunks_len);
	for (int i = 0; chunks_len > i; i++) {
		int width = (maze.width - i * CHUNK_SIZE) >= CHUNK_SIZE ? CHUNK_SIZE : maze.width % CHUNK_SIZE;
		chunks[i] = chunk_new(width, CHUNK_SIZE);
	}

	size_t buffer_len = sizeof(char) * maze.width + 2;
	char *buffer = malloc(buffer_len);

	int chunk_index = 0;
	int reading = 1;
	while (reading) {
		for (int i = 0; CHUNK_SIZE > i; i++) {
			int length = getline(&buffer, &buffer_len, maze_file);
			if (length == -1) {
				reading = 0;
				break;
			}
			if (buffer[length - 1] == '\n') {
				length--;
			}
			if (maze.width != length) {
				printf("Maze width is not consistent\n");
				exit(1);
			}

			for (int c = 0; length > c; c++) {
				char tile = ' ';
				switch (buffer[c]) {
					case 'X':
						tile = 'X';
					break;
					case 'P':
						maze.entry = maze.height * maze.width + c;
					break;
					case 'K':
						maze.exit = maze.height * maze.width + c;
					break;
					case '\n':
						printf("not gut\n");
					break;
				}
				chunks[c / CHUNK_SIZE].field[i][c % CHUNK_SIZE] = tile;
			}
			maze.height++;
		}

		int height = maze.height % CHUNK_SIZE == 0 ? CHUNK_SIZE : maze.height % CHUNK_SIZE;
		for (int i = 0; chunks_len > i; i++) {
			chunks[i].index = chunk_index;
			chunks[i].height = height;
			chunk_write(&chunks[i]);

			chunk_index++;
		}
	}
	free(buffer);
	fclose(maze_file);

	for (int i = 0; chunks_len > i; i++) {
		chunk_free(&chunks[i]);
	}
	free(chunks);

	return maze;
}

void maze_free(struct Maze maze) {
	DIR *temp = opendir("temp");
	if (temp == NULL) { 
		printf("Could not open current directory"); 
	}
	struct dirent *chunk;
	char chunk_file[19];
	while ((chunk = readdir(temp)) != NULL) {
		if (chunk->d_name[0] == '.')
			continue;
		sprintf(chunk_file, "temp/%s", chunk->d_name);
		if(remove(chunk_file) != 0) {
			printf("Could not remove %s\n", chunk_file);
		}
	}
	closedir(temp);     
	if(remove("temp") != 0) {
		printf("Could not remove temp folder\n");
	}
}
