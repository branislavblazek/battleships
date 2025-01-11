#include "player.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initializePlayer(Player* player, const char* name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    initializeGrid(&player->fleetGrid);
    initializeGrid(&player->trackingGrid);
    player->score = 0;   
}

void generateRandomFleet(Grid* grid, int shipSizes[], int shipsCount) {
    srand(time(NULL)); 

    for (int i = 0; i < shipsCount; i++) {
        int size = shipSizes[i];
        while (1) { 
            int r = rand() % GRID_SIZE;
            int s = rand() % GRID_SIZE;
            int isVertical = rand() % 2;
            if (placeShip(grid, r, s, size, isVertical) == 1) {
            break;
            }
        }
    }
}

int placeShip(Grid* grid, int startR, int startC,int size, int isVertical) {
    Ship ship = {size, startR, startC, isVertical, 0};
    if (isPlacementValid(grid, startR, startC, size, isVertical)) {
        grid->ships[grid->placedShipsCount] = ship;
        grid->placedShipsCount++;
        for (int i = 0; i < ship.size; i++) {
            int r = ship.startR + (ship.isVertical ? i : 0);
            int s = ship.startS + (ship.isVertical ? 0 : i);
            grid->cells[r][s] = SHIP;
        }
        
        return 1;
    }
    return 0;
}
