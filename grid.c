#include "grid.h"
#include <stdio.h>

void initializeGrid(Grid* grid) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid->cells[i][j] = EMPTY;
        }
    }
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

void printGrid(Grid* grid) {
    // Tlač hlavičky so stĺpcovými číslami
    printf("   ");
    for (int j = 0; j < GRID_SIZE; j++) {
        printf("%d ", j);
    }
    printf("\n");

    // Tlač mriežky s číslovaním riadkov
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i); // Tlač čísla riadku
        if (i < 10) printf(" "); // Zarovnanie pre jednociferné čísla

        for (int j = 0; j < GRID_SIZE; j++) {
            char cellSymbol;
            switch (grid->cells[i][j]) {
                case EMPTY: cellSymbol = '.'; break;
                case SHIP: cellSymbol = '#'; break;
                case HIT: cellSymbol = 'X'; break;
                case MISS: cellSymbol = 'O'; break;
                default: cellSymbol = '?'; break;
            }
            printf("%c ", cellSymbol);
        }
        printf("\n");
    }
}
