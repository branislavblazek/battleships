#include "grid.h"
#include <stdio.h>

void initializeGrid(Grid* grid) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid->cells[i][j] = EMPTY;
        }
    }
    grid->placedShipsCount = 0;
    
}

int isPlacementValid(Grid* grid, int startX, int startY, int size, int isVertical) {
    for (int i = 0; i < size; i++) {
        int x = startX + (isVertical ? i : 0);
        int y = startY + (isVertical ? 0 : i);

        if (x >= GRID_SIZE || y >= GRID_SIZE || grid->cells[x][y] != EMPTY) {
            return 0; // Neplatné umiestnenie
        }
    }
    return 1;
}

// Definície farieb pomocou ANSI escape sekvencií
#define RESET "\033[0m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"

void printGrid(Grid* grid) {
    // Tlač hlavičky so stĺpcovými číslami
    printf("   ");
    for (int j = 0; j < GRID_SIZE; j++) {
        printf(YELLOW "[%d]" RESET, j); // Stĺpce sú číslované od 1
    }
    printf("\n");

    // Tlač mriežky s riadkovými označeniami
    for (int i = 0; i < GRID_SIZE; i++) {
        printf(YELLOW "[%c]" RESET, 'A' + i); // Riadky označené písmenami

        for (int j = 0; j < GRID_SIZE; j++) {
            char* cellColor = RESET;
            char cellSymbol;

            switch (grid->cells[i][j]) {
                case EMPTY:
                    cellSymbol = '~';
                    cellColor = BLUE; // Prázdna voda je modrá
                    break;
                case SHIP:
                    cellSymbol = '#';
                    cellColor = GREEN; // Lode sú zelené
                    break;
                case HIT:
                    cellSymbol = 'X';
                    cellColor = RED; // Zásahy sú červené
                    break;
                case MISS:
                    cellSymbol = 'O';
                    cellColor = CYAN; // Chyby (miss) sú svetlomodré
                    break;
                default:
                    cellSymbol = '?';
                    break;
            }

            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }
        printf("\n");
    }

    // Tlač indikátorov lodí
    printf("\n" GREEN "Indicator:\n" RESET);
    printf(GREEN "Carrier [5]\n" RESET);
    printf(GREEN "Battleship [4]\n" RESET);
    printf(GREEN "Destroyer [3]\n" RESET);
    printf(GREEN "Submarine [3]\n" RESET);
    printf(GREEN "PatrolBoat [2]\n" RESET);
}

int isShipSunk(Grid* grid, Ship* ship) {
    for (int i = 0; i < ship->size; i++) {
        int r = ship->startR + (ship->isVertical ? i : 0);
        int s = ship->startS + (ship->isVertical ? 0 : i);

        if (grid->cells[r][s] != HIT) {
            return 0; // Loď nie je potopená
        }
    }
    ship->isSunk = 1; // Označ loď ako potopenú
    return 1;         // Loď je potopená
}

int isFleetDestroyed(Grid* grid) {
    for (int i = 0; i < 10; i++) {
        if (!grid->ships[i].isSunk) {
            return 0; // Hráč má ešte aktívnu loď
        }
    }
    return 1; // Všetky lode sú zničené
}


