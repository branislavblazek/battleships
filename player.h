#ifndef PLAYER_H
#define PLAYER_H

#include "grid.h"
#include "ship.h"

typedef struct {
    char name[50];      
    Grid fleetGrid;     // hracova flotila
    Grid trackingGrid;  // grid na sledovanie supera
    int score;          
} Player;


void initializePlayer(Player* player, const char* name);

int placeShip(Grid* grid, int startR, int startC,int size, int isVertical);

void generateRandomFleet(Grid* grid, int shipSizes[], int shipsCount);

#endif // PLAYER_H