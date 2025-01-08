#include "config.h"
#include "pipe.h"
#include "communication.h"
#include "player.h"
#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef struct {
  int fd_fifo_handshake_read;
  int fd_fifo_handshake_write;
  int fd_fifo_server_write;
  int fd_fifo_server_read;
} fd_fifo_client_struct;

void open_handshake(fd_fifo_client_struct *ffc) {
  ffc->fd_fifo_handshake_write = pipe_open_write(PIPE_HANDSHAKE_CLIENT_SERVER);
  ffc->fd_fifo_handshake_read = pipe_open_read(PIPE_HANDSHAKE_SERVER_CLIENT);
}

void connect_server(fd_fifo_client_struct *ffc) {
  int client_id;

  write(ffc->fd_fifo_handshake_write, "CONNECT", strlen("CONNECT"));

  read(ffc->fd_fifo_handshake_read, &client_id, sizeof(client_id));
  printf("Connected as Client %d.\n", client_id);

  if (client_id == 1) {
    ffc->fd_fifo_server_write = pipe_open_write(PIPE_CLIENT_1_SERVER);
    ffc->fd_fifo_server_read = pipe_open_read(PIPE_SERVER_CLIENT_1);
  } else if (client_id == 2) {
    ffc->fd_fifo_server_write = pipe_open_write(PIPE_CLIENT_2_SERVER);
    ffc->fd_fifo_server_read = pipe_open_read(PIPE_SERVER_CLIENT_2);
  }
}

void close_pipes(fd_fifo_client_struct *ffc) {
  pipe_close(ffc->fd_fifo_server_write);
  pipe_close(ffc->fd_fifo_server_read);
  pipe_close(ffc->fd_fifo_handshake_write);
  pipe_close(ffc->fd_fifo_handshake_read);
}

void game(fd_fifo_client_struct *ffc) {
  char buffer[BUFFER_SIZE];
  char enemyCoordsPrefix[3];
  char enemyCoordsData[3];
  int isStillWaiting = 0;

  while (1) {
    read_message(ffc->fd_fifo_server_read, buffer);

    int isMyTurn = strcmp(buffer, "TURN") == 0;
    int isWaiting = strcmp(buffer, "WAIT") == 0 || buffer[0] == '\0';
    int isExit = strcmp(buffer, "BYE") == 0;
    int isPossibleEnemyCoords = strlen(buffer) == 4;

    if (!isWaiting) {
      isStillWaiting = 0;
    }

    if (isMyTurn) {
      char coords[BUFFER_SIZE];
      printf("Enter coordinates for attack: ");
      scanf("%s", coords);
      send_message(ffc->fd_fifo_server_write, coords);
    } else if (isWaiting) {
      if (!isStillWaiting) {
        isStillWaiting = 1;
        printf("Wait for enemy...\n");
      }
    } else if (isExit) {
      return;
    } else if (isPossibleEnemyCoords) {
      strncpy(enemyCoordsPrefix, buffer, 2);

      enemyCoordsPrefix[2] = '\0';
      if (strcmp(enemyCoordsPrefix, "EN") == 0) {
        enemyCoordsData[0] = buffer[2];
        enemyCoordsData[1] = buffer[3];
        enemyCoordsData[2] = '\0';
        printf("Enemy's coords: %s\n", enemyCoordsData);
      } 
    }
  }
}

void createAndSendFleet(int fd_read, int fd_write, Player* player) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Veľkosti lodí
    int numShips = sizeof(shipSizes) / sizeof(shipSizes[0]);

    printf("Create your fleet:\n");

    for (int i = 0; i < numShips; i++) {
        int x, y, isVertical;

        while (1) {
            printf("Place ship of size %d (enter x y and 1 for vertical or 0 for horizontal): ", shipSizes[i]);
            scanf("%d %d %d", &x, &y, &isVertical);
            if (placeShip(&(player->fleetGrid),x,y,shipSizes[i],isVertical) == 1) {
              printf("Ship placed successfully.\n");
              break;
            } 
             printf("Invalid placement. Try again.\n");
        }
    }
    sendGridToServer(fd_write, &(player->fleetGrid));

}
void serializeGrid(Grid *grid, char *buffer) {
    
    int index = 0;

    // Serializácia buniek
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            buffer[index++] = (char)grid->cells[i][j]; // Konvertujeme CellState na char
        }
    }
    printf("serializacia hotov\n");

    // Serializácia počtu umiestnených lodí
    buffer[index++] = (char)grid->placedShipsCount;
    printf("serializacia poctu lodi\n");

    // Serializácia lodí (voliteľné)
    for (int i = 0; i < grid->placedShipsCount; i++) {
        memcpy(&buffer[index], &grid->ships[i], sizeof(Ship));
        index += sizeof(Ship);
    }
    printf("serializacia lodi\n");

    buffer[index] = '\0'; // Ukončenie reťazca
}



void sendGridToServer(int fd_write, Grid *grid) {
    char buffer[BUFFER_SIZE_GRID];
    printf("buffer hotov\n");
    serializeGrid(grid, buffer);
    write(fd_write, buffer, strlen(buffer) + 1);
}


void waitForStart(fd_fifo_client_struct *ffc) {
    char buffer[BUFFER_SIZE];
    while (1) {
        read_message(ffc->fd_fifo_server_read, buffer);
        if (strcmp(buffer, "START") == 0) {
            printf("Both players connected. You can now set up your fleet.\n");
            break;
        }
    }
}


void run_client() {
  fd_fifo_client_struct ffc = { -1, -1, -1, -1 };

  open_handshake(&ffc);

  if (ffc.fd_fifo_handshake_write == -1 || ffc.fd_fifo_handshake_read == -1) {
    perror("Error with connection to FIFO pipelines");
    exit(1);
  }

  connect_server(&ffc);

  if (ffc.fd_fifo_server_write == -1 || ffc.fd_fifo_server_read == -1) {
    perror("Error can't open FIFO pipelines");
    exit(1);
  }

  waitForStart(&ffc);
  
  Player player;
  initializePlayer(&player, "PLAYER1");

  // klient vytvorit flotilu a posle ju serveru
  createAndSendFleet(ffc.fd_fifo_server_read, ffc.fd_fifo_server_write, &player);
  
  game(&ffc);

  // int num;
  // while (1) {
  //     printf("Zadajte číslo na odoslanie: ");
  //     scanf("%d", &num);

  //     if (write(ffc.fd_fifo_server_write, &num, sizeof(int)) == -1) {
  //         perror("Chyba pri odoslaní čísla");
  //         break;
  //     }

  //     if (read(ffc.fd_fifo_server_read, &num, sizeof(int)) > 0) {
  //         printf("Prijaté číslo: %d\n", num);
  //     } else {
  //         printf("Spojenie so serverom prerušené.\n");
  //         break;
  //     }
  // }

  close_pipes(&ffc);
}
