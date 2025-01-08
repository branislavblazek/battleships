#ifndef GRID_H
#define GRID_H

#define GRID_SIZE 10

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

// Funkcie na manipuláciu s hracou plochou
void initializeGrid(Grid* grid);
int isPlacementValid(Grid* grid, int startR, int startS, int size, int isVertical);
void printGrid(Grid* grid);
int isShipSunk(Grid* grid, Ship* ship);
int isFleetDestroyed(Grid* grid);

#endif // GRID_H