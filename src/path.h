#ifndef PATH_H
struct Node{
    int nodeX;
    int nodeY;
    int lenght;
    char rotation;
    struct Node *previous;
};

struct Point{
    int X;
    int Y;
    char rotation;
};


struct Node pathFinder(struct Maze *maze);

void pathGet(struct Node *nodes);

#endif 