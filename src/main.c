#include <stdio.h>

#include "maze.h"

int main(int argc, char *argv[])
{
	char *path = argc > 1 ? argv[1] : "maze.txt";
	struct Maze maze = maze_load(path);
	maze_free(&maze);
	return 0;
}
