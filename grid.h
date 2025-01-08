#ifndef GRID_H
#define GRID_H

#define GRID_SIZE 10

#define RESET "\033[0m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"

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