#ifndef PLAYER_H
#define PLAYER_H

#include "grid.h"
#include "ship.h"

typedef struct {
    char name[50];      // Meno hráča
    Grid fleetGrid;     // Grid s jeho flotilou
    Grid trackingGrid;  // Grid na sledovanie súpera
    Ship ships[10];     // Zoznam lodí (1x4, 2x3, 3x2, 4x1)
    int score;          // Počet zničených lodí súpera
} Player;

// Funkcie pre hráča
void initializePlayer(Player* player, const char* name);
void printPlayerGrid(Player* player);
int isFleetDestroyed(Player* player);
int shoot(Grid* opponentGrid, Grid* trackingGrid, int r, int s);

void generateRandomFleet(Player* player);

#endif // PLAYER_H