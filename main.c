#include "server.h"
#include "client.h"
#include "pipe.h"
#include "config.h"
#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
/*
void testShooting(Player* player, Grid* trackingGrid) {
    int r, s;

    printf("\nEnter coordinates to shoot (row, column): ");
    scanf("%d %d", &r, &s);

    // Over, či sú súradnice v rámci hracej plochy
    if (r < 0 || r >= GRID_SIZE || s < 0 || s >= GRID_SIZE) {
        printf("Invalid coordinates! Try again.\n");
        return;
    }

    // Streľba
    int result = shoot(&player->fleetGrid, trackingGrid, r, s);
    if (result == 1) {
        printf("Hit!\n");

        // Skontroluj, či bola loď potopená
        for (int i = 0; i < 10; i++) {
            if (isShipSunk(&player->fleetGrid, &player->ships[i])) {
                printf("You sunk a ship!\n");
            }
        }
    } else if (result == 0) {
        printf("Miss!\n");
    } else {
        printf("Invalid shot! You've already shot here.\n");
    }
}
*/
 int main(int argc, char* argv[]) {
   if (argc == 2 && strcmp(argv[1], "server") == 0) {
     run_server();
    } else {
      run_client();
      wait(NULL);
   }

    return 0;
 }


/*
int main() {
    // Inicializácia hráča
    Player player;
    initializePlayer(&player, "Player 1");

    // Generovanie náhodnej flotily
    generateRandomFleet(&player);

    // Výpis hracej plochy hráča
    printf("\n--- Player's Fleet ---\n");
    printPlayerGrid(&player);

    // Streľba na flotilu hráča, kým nie sú všetky lode potopené
    while (!isFleetDestroyed(&player)) {
        printf("\n--- Your turn ---\n");
        testShooting(&player, &player.trackingGrid);

        // Výpis aktuálneho stavu mriežky
        printf("\n--- Player's Fleet ---\n");
        printPlayerGrid(&player);
    }

    // Oznámenie konca hry
    printf("\nAll ships have been destroyed! Game over.\n");

    return 0;
}
*/
