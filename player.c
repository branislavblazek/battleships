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

// Výpis hráčovej flotily
void printPlayerGrid(Player* player) {
    printf("\n%s's Fleet Grid:\n", player->name);
    printGrid(&player->fleetGrid);

    printf("\n%s's Tracking Grid:\n", player->name);
    printGrid(&player->trackingGrid);
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

void generateRandomFleet(Grid* grid) {
    srand(time(NULL)); 

    //TODO inicializovat velkosti lodi univerzalne
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; 

    for (int i = 0; i < 10; i++) {
        int size = shipSizes[i];
        while (1) { 
            int r = rand() % GRID_SIZE;
            int s = rand() % GRID_SIZE;
            int isVertical = rand() % 2;
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
        
        return 1;
    }
    return 0;
}

void printGridsSideBySide(Grid* trackingGrid, Grid* userGrid) {
    // Hlavičky stĺpcov
    printf("   ");
    for (int j = 0; j < GRID_SIZE; j++) {
        printf(YELLOW "[%d]" RESET, j + 1);
    }
    printf("      ");
    for (int j = 0; j < GRID_SIZE; j++) {
        printf(YELLOW "[%d]" RESET, j + 1);
    }
    printf("\n");

    // Riadky mriežky
    for (int i = 0; i < GRID_SIZE; i++) {
        // Riadková hlavička pre trackingGrid
        printf(YELLOW "[%c]" RESET, 'A' + i);

        // Zobrazujeme trackingGrid
        for (int j = 0; j < GRID_SIZE; j++) {
            char* cellColor = RESET;
            char cellSymbol;

            switch (trackingGrid->cells[i][j]) {
                case EMPTY:
                    cellSymbol = '~';
                    cellColor = BLUE; // Prázdne pole
                    break;
                case HIT:
                    cellSymbol = 'X';
                    cellColor = RED; // Zásah
                    break;
                case MISS:
                    cellSymbol = 'O';
                    cellColor = CYAN; // Minul cieľ
                    break;
                default:
                    cellSymbol = '?';
                    break;
            }
            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }

        // Medzera medzi mriežkami
        printf("      ");

        // Riadková hlavička pre userGrid
        printf(YELLOW "[%c]" RESET, 'A' + i);

        // Zobrazujeme userGrid
        for (int j = 0; j < GRID_SIZE; j++) {
            char* cellColor = RESET;
            char cellSymbol;

            switch (userGrid->cells[i][j]) {
                case EMPTY:
                    cellSymbol = '~';
                    cellColor = BLUE; // Prázdne pole
                    break;
                case SHIP:
                    cellSymbol = '#';
                    cellColor = GREEN; // Loď
                    break;
                case HIT:
                    cellSymbol = 'X';
                    cellColor = RED; // Zásah
                    break;
                case MISS:
                    cellSymbol = 'O';
                    cellColor = CYAN; // Minul cieľ
                    break;
                default:
                    cellSymbol = '?';
                    break;
            }
            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }

        // Ukončenie riadku
        printf("\n");
    }

    // Tlač indikátorov
    printf("\n" GREEN "Tracking Grid (Enemy):\n" RESET);
    printf(CYAN "~ = Untouched\n" RESET);
    printf(RED "X = Hit\n" RESET);
    printf(CYAN "O = Miss\n" RESET);

    printf("\n" GREEN "Your Fleet:\n" RESET);
    printf(GREEN "Carrier [5]\n" RESET);
    printf(GREEN "Battleship [4]\n" RESET);
    printf(GREEN "Destroyer [3]\n" RESET);
    printf(GREEN "Submarine [3]\n" RESET);
    printf(GREEN "PatrolBoat [2]\n" RESET);
}


