#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h> 
#include <sys/stat.h>

#include "maze.h"
#include "chunk.h"


struct Maze maze_new(void) {
	struct Maze maze;
	maze.width = 0;
	maze.height = 0;
	maze.exit = -1;
	maze.exit = -1;
	maze.type = unknown;
	maze.path = NULL;
        mkdir("temp", 0755);

	maze.chunk_belt = malloc(sizeof(struct Chunk) * BELT_SIZE);
	for (int i = 0; BELT_SIZE > i; i++) {
		maze.chunk_belt[i].index = -1;
		maze.chunk_belt[i].field = NULL;
	}
	fclose(chunk_file);
	free(buffer);

	return chunk;
}

struct Maze load_txt(char *maze_path) {
	FILE *maze_file = fopen(maze_path, "r");
	if (maze_file == NULL) {
		printf("Could not open maze file: %s\n", maze_path);
		exit(1);
	}

	struct Maze maze = maze_new();

	while (fgetc(maze_file) != '\n') {
		maze.width += 1;
	}
	rewind(maze_file);

	// Amount of chunks needed to load whole row
	struct Belt belt = belt_new(maze.width, CHUNK_SIZE);

	size_t buffer_len = sizeof(char) * maze.width + 2;
	char *buffer = malloc(buffer_len);

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
				char tile = TILE;
				switch (buffer[c]) {
					case 'X':
						tile += WALL;
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
				belt_set(&belt, c, i, tile);
			}
			maze.height++;
		}

		int height = maze.height % CHUNK_SIZE == 0 ? CHUNK_SIZE : maze.height % CHUNK_SIZE;
		belt_write(&belt, height);
	}
	free(buffer);
	fclose(maze_file);
	belt_free(&belt);

	return maze;

}

struct Maze load_bin(char *maze_path) {
	FILE *maze_file = fopen(maze_path, "rb");
	if (maze_file == NULL) {
		printf("Could not open maze file: %s\n", maze_path);
		exit(1);
	}

	short width; // columns
	short height; // rows
	short entry_x;
	short entry_y;
	short exit_x;
	short exit_y;
	int counter;
	char wall;
	char path;

	fseek(maze_file, 5, SEEK_CUR);
	fread(&width, 2, 1, maze_file);
	fread(&height, 2, 1, maze_file);
	fread(&entry_x, 2, 1, maze_file);
	fread(&entry_y, 2, 1, maze_file);
	fread(&exit_x, 2, 1, maze_file);
	fread(&exit_y, 2, 1, maze_file);
	fseek(maze_file, 12, SEEK_CUR);
	fread(&counter, 4, 1, maze_file);
	fseek(maze_file, 5, SEEK_CUR);
	fread(&wall, 1, 1, maze_file);
	fread(&path, 1, 1, maze_file);

	struct Maze maze = maze_new();
	maze.width = width;
	maze.height = height;

	entry_x--;
	entry_y--;
	exit_x--;
	exit_y--;
	maze.entry = width * entry_y + entry_x;
	maze.exit = width * exit_y + exit_y;

	struct Belt belt = belt_new(width, CHUNK_SIZE);

	int chunks_x = 0;
	int chunks_y = 0;
	char value;
	unsigned char count;
	for (int i = 0; counter > i; i++) {
		fseek(maze_file, 1, SEEK_CUR);
		fread(&value, 1, 1, maze_file);
		fread(&count, 1, 1, maze_file);
		int v = TILE;
		if (value == wall) {
			v |=  WALL;
		} else if (value != path) {
			printf("Invalid value encounterd while parsing binary input\n");
			exit(1);
		}

		for (int c = 0; count >= c; c++) {
			belt_set(&belt, chunks_x, chunks_y, v);
			chunks_x++;
		}
		if (chunks_x > width) {
			printf("Invalid binary encoding: row spillout\n");
			exit(1);
		}
		if (chunks_x == width) {
			chunks_y++;
			chunks_x = 0;
		}
		if (chunks_y == CHUNK_SIZE) {
			belt_write(&belt, CHUNK_SIZE);
			chunks_y = 0;
		}
	}
	// Save final chunks
	belt_write(&belt, chunks_y);

	fclose(maze_file);
	belt_free(&belt);

	return maze;
}

enum file_type get_type(char *maze_path) {
	int last_dot = 0;
	int i = 0;
	while (maze_path[i] != '\0') {
		if (maze_path[i] == '.') {
			last_dot = i;
		}
		i++;
	}

	if (!strcmp(&maze_path[last_dot], ".txt")) {
		return txt;
	} else if (!strcmp(&maze_path[last_dot], ".bin")) {
		return bin;
	} else {
		return unknown;
	}
}

struct Maze maze_load(char *maze_path) {
	enum file_type type = get_type(maze_path);
	struct Maze maze;
	switch (type) {
		case unknown:
			printf("Invalid maze type\n");
			exit(1);
		case bin:
			maze = load_bin(maze_path);
			break;
		case txt:
			maze = load_txt(maze_path);
			break;
	}
	maze.path = maze_path;
	maze.type = type;
	return maze;
}

void maze_free(struct Maze *maze) {
	for (int i = 0; BELT_SIZE > i; i++) {
		chunk_free(&maze->chunk_belt[i]);
	}
	free(maze->chunk_belt);

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

char *maze_get(struct Maze *maze, int x, int y) {
	if (x >= maze->width || y >= maze->height || 0 > x || 0 > y) {
		printf("Access out of bounds: %i, %i", x, y);
		exit(1);
	}
	// gets index of chunk based on (x, y) coordinates
	int chunk_index = (maze->width + CHUNK_SIZE - 1) / CHUNK_SIZE * (y / CHUNK_SIZE) + x / CHUNK_SIZE;
	int relative_x = x - x / CHUNK_SIZE * CHUNK_SIZE;
	int relative_y = y - y / CHUNK_SIZE * CHUNK_SIZE;

	for (int i = 0; BELT_SIZE > i; i++) {
		if (maze->chunk_belt[i].index == chunk_index) {
			if (i > 0) {
				struct Chunk chunk = maze->chunk_belt[i];
				memmove(maze->chunk_belt + 1, maze->chunk_belt, sizeof(struct Chunk) * i);
				maze->chunk_belt[0] = chunk;
			}
			return &maze->chunk_belt[0].field[relative_y][relative_x];
		}
	}
	chunk_write(&maze->chunk_belt[BELT_SIZE - 1]);
	chunk_free(&maze->chunk_belt[BELT_SIZE - 1]);
	memmove(maze->chunk_belt + 1, maze->chunk_belt, sizeof(struct Chunk) * (BELT_SIZE - 1));
	maze->chunk_belt[0] = chunk_load(chunk_index);
	return &maze->chunk_belt[0].field[relative_y][relative_x];
}

