#include "grid.h"
#include <stdio.h>
#include <string.h>

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

        // kontrola ci lodka nie je mimo gridu
        if (x >= GRID_SIZE || y >= GRID_SIZE || grid->cells[x][y] != EMPTY) {
            return 0;
        }

        // kontrola susedov, kvoli medzere
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx;
                int ny = y + dy;

                // ak sused je vramci gridu
                if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                    // ak susedna bunka je lod, nemozno umiestnit
                    if (grid->cells[nx][ny] == SHIP) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}


void clearScreen() {
  printf(CLEAR_SCREEN);
  printf("\033[H");
  fflush(stdout);
}



int isShipSink(Grid* grid, int xHit, int yHit) {
  if (xHit < 0 || xHit >= GRID_SIZE || yHit < 0 || yHit >= GRID_SIZE || grid->cells[xHit][yHit] != HIT) {
    return 0;
  }

  int start_row = xHit;
  int end_row = xHit;
  int start_col = yHit;
  int end_col = yHit;

  while (start_col > 0 && grid->cells[xHit][start_col - 1] == SHIP) (start_col)--;
  while (end_col < GRID_SIZE - 1 && grid->cells[xHit][end_col + 1] == SHIP) (end_col)++;

  if (start_col == end_col) {
    while (start_row > 0 && grid->cells[start_row - 1][yHit] == SHIP) (start_row)--;
    while (end_row < GRID_SIZE - 1 && grid->cells[end_row + 1][yHit] == SHIP) (end_row)++;
  }

  for (int r = start_row; r <= end_row; r++) {
    for (int c = start_col; c <= end_col; c++) {
      if (grid->cells[r][c] == SHIP) {
        return 0;
      }
    }
  }

  return 1;
}

void getMissAroundSinkShip(Grid* grid, int xHit, int yHit, char* result) {
  int start_row = xHit;
  int end_row = xHit;
  int start_col = yHit;
  int end_col = yHit;

  while (start_col > 0 && grid->cells[xHit][start_col - 1] == HIT) start_col--;
  while (end_col < GRID_SIZE - 1 && grid->cells[xHit][end_col + 1] == HIT) end_col++;

  if (start_col == end_col) {
    while (start_row > 0 && grid->cells[start_row - 1][yHit] == HIT) start_row--;
    while (end_row < GRID_SIZE - 1 && grid->cells[end_row + 1][yHit] == HIT) end_row++;
  }
  
  for (int r = start_row - 1; r <= end_row + 1; r++) {
    for (int c = start_col - 1; c <= end_col + 1; c++) {
      if (r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE) {
        if (r >= start_row && r <= end_row && c >= start_col && c <= end_col) {
          continue;
        }

        if (grid->cells[r][c] == EMPTY) {
          char buffer[5];
          sprintf(buffer, "%d%d ", r, c);
          strcat(result, buffer);
        }
      }
    }
  }
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
