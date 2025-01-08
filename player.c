#include <string.h>
#include <stdio.h>
#include "player.h"

// Inicializácia hráča
void initializePlayer(Player* player, const char* name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    initializeGrid(&player->fleetGrid);
    initializeGrid(&player->trackingGrid);
    player->score = 0;
    
}

// Výpis hráčovej flotily - seba alebo nepriateľa
void printPlayerGrid(Player* player) {
    printf("\n%s's Fleet Grid:\n", player->name);
    printGrid(&player->fleetGrid);

    printf("\n%s's Tracking Grid:\n", player->name);
    printGrid(&player->trackingGrid);
}

// Kontrola, či sú všetky lode hráča zničené


int shoot(Grid* opponentGrid, Grid* trackingGrid, int x, int y) {
    if (opponentGrid->cells[x][y] == SHIP) {
        opponentGrid->cells[x][y] = HIT;
        trackingGrid->cells[x][y] = HIT;
        return 1; // Zásah
    } else if (opponentGrid->cells[x][y] == EMPTY) {
        opponentGrid->cells[x][y] = MISS;
        trackingGrid->cells[x][y] = MISS;
        return 0; // Mimo
    }
    return -1; // Už strelené políčko
}

#include "player.h"
#include <stdlib.h>
#include <time.h>

void generateRandomFleet(Grid* grid) {
    srand(time(NULL)); // Inicializácia RNG

    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Veľkosti lodí

    for (int i = 0; i < 10; i++) {
        int size = shipSizes[i];
        while (1) { // Pokúšaj sa umiestniť loď, kým nenájdeš platné miesto
            int r = rand() % GRID_SIZE;
            int s = rand() % GRID_SIZE;
            int isVertical = rand() % 2;

            Ship ship = {size, r, s, isVertical, 0}; // Vytvor novú loď
            placeShip(grid, r, s, size, isVertical);
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
        printGrid(grid);
        return 1;
    }
    return 0;
    
    
}
