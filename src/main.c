#include "maze.h"
#include "solver.h"

int main(int argc, char *argv[])
{
	char *path = argc > 1 ? argv[1] : "maze.txt";
	struct Maze maze = maze_load(path);

	// Solve the maze and add solution to the structure
	maze_solve(&maze);
	// Write the solution to desired output
	maze_solution(&maze);
	maze_free(&maze);
	return 0;
}
