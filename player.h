#ifndef PLAYER_H
#define PLAYER_H

#include "grid.h"
#include "ship.h"

typedef struct {
    char name[50];      // Meno hráča
    Grid fleetGrid;     // Grid s jeho flotilou
    Grid trackingGrid;  // Grid na sledovanie súpera
    int score;          // Počet zničených lodí súpera
} Player;


void initializePlayer(Player* player, const char* name);

int placeShip(Grid* grid, int startR, int startC,int size, int isVertical);

void generateRandomFleet(Grid* grid);

#endif // PLAYER_H