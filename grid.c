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

void clearScreen() {
  printf(CLEAR_SCREEN);
  printf("\033[H");
  fflush(stdout);
}



// TODO: do we really needs this one?
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

int checkShips(Grid* grid) {
  int shipCount = 0;

  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      if (grid->cells[i][j] == SHIP) {
        shipCount++;
      }
    }
  }

  return shipCount;
}
