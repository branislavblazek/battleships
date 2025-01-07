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
int isFleetDestroyed(Player* player) {
    for (int i = 0; i < 10; i++) {
        if (!player->ships[i].isSunk) {
            return 0; // Hráč má ešte aktívnu loď
        }
    }
    return 1; // Všetky lode sú zničené
}

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

void generateRandomFleet(Player* player) {
    srand(time(NULL)); // Inicializácia RNG

    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Veľkosti lodí

    for (int i = 0; i < 10; i++) {
        int size = shipSizes[i];
        while (1) { // Pokúšaj sa umiestniť loď, kým nenájdeš platné miesto
            int r = rand() % GRID_SIZE;
            int s = rand() % GRID_SIZE;
            int isVertical = rand() % 2;

            Ship ship = {size, r, s, isVertical, 0}; // Vytvor novú loď

            if (isPlacementValid(&player->fleetGrid, r, s, size, isVertical)) {
                placeShip(&player->fleetGrid, &ship);
                player->ships[i] = ship; // Ulož loď do zoznamu hráča
                break;
            }
        }
    }
}
