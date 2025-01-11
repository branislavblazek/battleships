#ifndef GRID_H
#define GRID_H

#define GRID_SIZE       10
#define RESET           "\033[0m"
#define BLUE            "\033[34m"
#define GREEN           "\033[32m"
#define RED             "\033[31m"
#define YELLOW          "\033[33m"
#define CYAN            "\033[36m"
#define CLEAR_SCREEN    "\033[2J"
#define RESET_COLOR     "\033[0m"

#include "ship.h"

typedef enum {
    EMPTY,
    SHIP,
    HIT,
    MISS
} CellState;

typedef struct {
    CellState cells[GRID_SIZE][GRID_SIZE];
    int placedShipsCount;
    Ship ships[10];
} Grid;

void initializeGrid(Grid* grid);

int isPlacementValid(Grid* grid, int startR, int startS, int size, int isVertical);

void printGrid(Grid* grid);

void printDoubleGrid2(Grid* grid1, Grid* grid2);

int isShipSink(Grid* grid, int xHit, int yHit);

void getMissAroundSinkShip(Grid* grid, int xHit, int yHit, char* buffer);

int isFleetDestroyed(Grid* grid);

int checkShips(Grid* grid);

#endif // GRID_H
