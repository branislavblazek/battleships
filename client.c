#include "config.h"
#include "pipe.h"
#include "communication.h"
#include "player.h"
#include "client.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

typedef struct {
  int fd_fifo_handshake_read;
  int fd_fifo_handshake_write;
  int fd_fifo_server_write;
  int fd_fifo_server_read;
} fd_fifo_client_struct;

void open_client_handshake(fd_fifo_client_struct *ffc) {
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

void close_client_pipes(fd_fifo_client_struct *ffc) {
  pipe_close(ffc->fd_fifo_server_write);
  pipe_close(ffc->fd_fifo_server_read);
  pipe_close(ffc->fd_fifo_handshake_write);
  pipe_close(ffc->fd_fifo_handshake_read);
}

void process_turn(char *buffer, Player *player) {
  // XY1A.M
    //XY-prefix;
    //1A-coords;
    //./X-water/hit;
    //M/E-me/enemy
  char coordsPrefix[3];
  char coordsData[3];
  int x, y, attack;

  strncpy(coordsPrefix, buffer, 2);
  coordsPrefix[2] = '\0';
  if (strcmp(coordsPrefix, "XY") == 0) {
    coordsData[0] = buffer[2];
    coordsData[1] = buffer[3];
    coordsData[2] = '\0';

    int ok = parseInput(coordsData, &x, &y);
    if (!ok) {
      printf("Can't decode coords from server: %s\n", coordsData);
      return;
    }

    printf("Recieved %s\n", buffer);
    
    attack = buffer[4] == '.' ? MISS : HIT;
    if (buffer[5] == 'E') {
      player->fleetGrid.cells[x][y] = attack;
    } else {
      player->trackingGrid.cells[x][y] = attack;
    }
  }
}

void* threadRendering(void* arg) {
  Player *player = arg;

  printDoubleGrid(&player->trackingGrid, &player->fleetGrid);

  return NULL;
}

void game(fd_fifo_client_struct *ffc, Player *player) {
    pthread_t render_thread = 0; // Inicializacia vlakna, bez toho to pada
    char buffer[BUFFER_SIZE], coordsUser[BUFFER_SIZE];
    int isStillWaiting = 0;

    while (1) {
        int ok = readMessage(ffc->fd_fifo_server_read, buffer);

        if (ok == 1) { // disconnected
            puts("Exit due to server disconnected.");
            break;
        }

        int isMyTurn = strcmp(buffer, "TURN") == 0;
        int isWaiting = strcmp(buffer, "WAIT") == 0;
        int isWin = strcmp(buffer, "WIN") == 0;
        int isLost = strcmp(buffer, "LOST") == 0;
        int isExit = strcmp(buffer, "BYE") == 0;
        int isPossibleCoords = strlen(buffer) == 6;

        if (!isWaiting) {
            isStillWaiting = 0;
        }

        if (isMyTurn) {
            //ATTACK
            //neviem ci tu musi byt, ide aj bez toho ale nesledovala som to 
            pthread_join(render_thread, NULL); 
            //render_thread = 0;
            printf("Enter coordinates for attack: ");
            scanf("%s", coordsUser);
            sendMessage(ffc->fd_fifo_server_write, coordsUser);
        } else if (isWaiting && !isStillWaiting) {
            isStillWaiting = 1;

        } else if (isPossibleCoords) {
            process_turn(buffer, player);
            //if (render_thread) {
                //pthread_join(render_thread, NULL); // Ukonči predošlé vlákno
            //}
            Player tempPlayer = *player;
            pthread_create(&render_thread, NULL, threadRendering,  &tempPlayer);
            pthread_detach(render_thread);

        } else if (isWin) {
            puts("YOU ARE WINEEER!");
            break;
        } else if (isLost) {
            puts("HA LOOSER!");
            break;
        } else if (isExit) {
            return;
        }
    }
    //if (render_thread) {
       // pthread_join(render_thread, NULL); // Ukonči posledné vlákno
    //}
}


void createAndSendFleet(int fd_read, int fd_write, Player* player) {
  //int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
  int shipSizes[] = { 4 };
  int shipsCount = sizeof(shipSizes) / sizeof(shipSizes[0]);

  printf("Create your fleet:\n");

  for (int i = 0; i < shipsCount; i++) {
    char input[BUFFER_SIZE];
    int x, y, isVertical;

    while (1) {
      printf("Place ship of size %d. Enter position (e.g., A0 0): ", shipSizes[i]);
      scanf("%s %d", input, &isVertical);
      if (isVertical != 1 && isVertical != 0) {
        printf("Invalid input for vertical/horizontal. Choose 1 for vertical or 0 for horizontal .\n");
        continue;
      }

      int ok = parseInput(input, &x, &y);
      if (!ok) continue;

      if (placeShip(&(player->fleetGrid), x, y, shipSizes[i], isVertical) == 1) {
        printGrid(&(player->fleetGrid));
        printf("Ship placed successfully.\n");
        break;
      }

      printf("Invalid placement. Try again.\n");
    }
  }
  char buffer[BUFFER_SIZE_GRID];
  memset(buffer, 0, sizeof(buffer));

  // serializacia flotily
  serializeFleet(player, buffer, shipsCount);

  // odoslanie flotily serveru
  write(fd_write, buffer, BUFFER_SIZE_GRID);

  // potvrdenie od servera
  char response[BUFFER_SIZE];
  int attempts = 0;

  while (attempts < 3) {
    readMessage(fd_read, response);
    
    if (strcmp(response, "FLEET_OK") == 0) {
      printf("Server confirmed fleet reception.\n");
      break;
    }

    attempts++;
  }
    
  if (attempts == 3) {
    printf("Server reported an issue with the fleet. Please retry.\n");
  }  
}

void serializeFleet(Player* player, char* buffer, int shipsCount) {
  int index = 0;

  // serveru sa odosielaju iba lode
  for (int i = 0; i < shipsCount; i++) {
    int x = player->fleetGrid.ships[i].startR;
    int y = player->fleetGrid.ships[i].startS;
    int isVertical = player->fleetGrid.ships[i].isVertical;
    int size = player->fleetGrid.ships[i].size;

    // pridanie lodi do bufferu
    index += sprintf(&buffer[index], "%d %d %d %d ", x, y, isVertical, size);
  }

  buffer[index - 1] = '\0'; // nahradenie poslednej medzery ukoncovacim znakom
}

void waitForStart(fd_fifo_client_struct *ffc) {
  char buffer[BUFFER_SIZE];
  while (1) {
    readMessage(ffc->fd_fifo_server_read, buffer);
    if (strcmp(buffer, "START") == 0) {
      printf("Both players connected. You can now set up your fleet.\n");
      break;
    }
  }
}

void run_client() {
  fd_fifo_client_struct ffc = { -1, -1, -1, -1 };

  open_client_handshake(&ffc);

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

  printf("strarting game(ffc)\n");

  game(&ffc, &player);

  close_client_pipes(&ffc);
}
