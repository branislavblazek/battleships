#include "ship.h"
#include "grid.h"

void placeShip(Grid* grid, Ship* ship) {
    for (int i = 0; i < ship->size; i++) {
        int r = ship->startR + (ship->isVertical ? i : 0);
        int s = ship->startS + (ship->isVertical ? 0 : i);
        grid->cells[r][s] = SHIP;
    }
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
