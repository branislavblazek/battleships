#include "config.h"
#include "pipe.h"
#include "communication.h"

#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>


typedef struct {
  int fd_fifo_handshake_read;
  int fd_fifo_handshake_write;
  int fd_fifo_client_1_read;
  int fd_fifo_client_1_write;
  int fd_fifo_client_2_read;
  int fd_fifo_client_2_write;
} fd_fifo_server_struct;

void init() {
  pipe_init(PIPE_HANDSHAKE_CLIENT_SERVER);
  pipe_init(PIPE_HANDSHAKE_SERVER_CLIENT);
  pipe_init(PIPE_CLIENT_1_SERVER);
  pipe_init(PIPE_SERVER_CLIENT_1);
  pipe_init(PIPE_CLIENT_2_SERVER);
  pipe_init(PIPE_SERVER_CLIENT_2);
}

void destroy() {
  pipe_destroy(PIPE_HANDSHAKE_CLIENT_SERVER);
  pipe_destroy(PIPE_HANDSHAKE_SERVER_CLIENT);
  pipe_destroy(PIPE_CLIENT_1_SERVER);
  pipe_destroy(PIPE_SERVER_CLIENT_1);
  pipe_destroy(PIPE_CLIENT_2_SERVER);
  pipe_destroy(PIPE_SERVER_CLIENT_2);
}

void open_handshake(fd_fifo_server_struct *ffs) {
  ffs->fd_fifo_handshake_read = pipe_open_read(PIPE_HANDSHAKE_CLIENT_SERVER);
  ffs->fd_fifo_handshake_write = pipe_open_write(PIPE_HANDSHAKE_SERVER_CLIENT);
}

void connect_clients(fd_fifo_server_struct *ffs) {
  int client_count = 0;

  while (client_count < 2) {
    char buffer[16];
    int bytes_read = read(ffs->fd_fifo_handshake_read, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      if (strcmp(buffer, "CONNECT") == 0) {
        client_count++;
        write(ffs->fd_fifo_handshake_write, &client_count, sizeof(client_count));

        if (client_count == 1) {
          ffs->fd_fifo_client_1_read = pipe_open_read(PIPE_CLIENT_1_SERVER);
          ffs->fd_fifo_client_1_write = pipe_open_write(PIPE_SERVER_CLIENT_1);
          printf("Client 1 connected.\n");
        } else if (client_count == 2) {
          ffs->fd_fifo_client_2_read = pipe_open_read(PIPE_CLIENT_2_SERVER);
          ffs->fd_fifo_client_2_write = pipe_open_write(PIPE_SERVER_CLIENT_2);
          printf("Client 2 connected.\n");
        }
      }
    }
  }
}

void close(fd_fifo_server_struct *ffs) {
  pipe_close(ffs->fd_fifo_client_1_read);
  pipe_close(ffs->fd_fifo_client_1_write);
  pipe_close(ffs->fd_fifo_client_2_read);
  pipe_close(ffs->fd_fifo_client_2_write);
  pipe_close(ffs->fd_fifo_handshake_read);
  pipe_close(ffs->fd_fifo_handshake_write);
}

void message_turn(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE];
  strcpy(message, "TURN");

  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, message);
}

void message_wait(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE];
  strcpy(message, "WAIT");
  
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, message);
}

void message_bye(fd_fifo_server_struct *ffs) {
  char message[BUFFER_SIZE];
  strcpy(message, "BYE");
  
  send_message(ffs->fd_fifo_client_1_write, message);
  send_message(ffs->fd_fifo_client_2_write, message);
}

int read_turn(fd_fifo_server_struct *ffs, int turn, char *coords) {
  char buffer[BUFFER_SIZE];

  int fd = turn == 0 
    ? ffs->fd_fifo_client_1_read
    : ffs->fd_fifo_client_2_read;

  read_message(fd, buffer);

  if (buffer[0] == '\0') {
    return 0;
  }
  
   
  strncpy(coords, buffer, 2);
  coords[2] = '\0';
  return 1;
}

void message_turn_data(fd_fifo_server_struct *ffs, int turn, char *data) {
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, data);
}

void game(fd_fifo_server_struct *ffs, Grid *fleetGrid1, Grid *fleetGrid2) {
  char coords[3];
  char message[BUFFER_SIZE];
  int turn = 0;
  // will be removed....
  int MAX_ATTEMPS = 8;
  int attemps= 0; 
  


  while (attemps < MAX_ATTEMPS) {
    message_turn(ffs, turn);
    message_wait(ffs, 1 - turn);

    int ok = read_turn(ffs, turn, coords);
    if (!ok) {
      continue;
    }

    printf("Got _%s_ from client %d\n", coords, turn + 1);

    sprintf(message, "EN%s", coords);
    message_turn_data(ffs, 1 - turn, message);

    usleep(1000);
    

    turn = 1 - turn;
    attemps++;
  }

  message_bye(ffs);
}


#include <pthread.h>
#include "server.h"

typedef struct {
    fd_fifo_server_struct* ffs;
    int client_id;
    Grid* fleetGrid;
} FleetThreadArgs;


void receiveFleetParallel(fd_fifo_server_struct* ffs, Grid* grid1, Grid* grid2) {
    printf("starting receiveFleetParallel");
    pthread_t thread1, thread2;
    FleetThreadArgs args1 = {ffs, 1, grid1};
    FleetThreadArgs args2 = {ffs, 2, grid2};

    // dve paralelne vlakna na zvolenie flotily
    pthread_create(&thread1, NULL, receiveFleetThread, &args1);
    pthread_create(&thread2, NULL, receiveFleetThread, &args2);

    // synchornizacia vlakien
    pthread_join(thread1, NULL);
    printf("Fleet for Client 1 received successfully.\n");
    pthread_join(thread2, NULL);
    printf("Fleet for Client 2 received successfully.\n");
    
    printf("Both fleets have been received.\n");
}

void notifyClientsToStart(fd_fifo_server_struct *ffs) {
    char message[BUFFER_SIZE] = "START";
    send_message(ffs->fd_fifo_client_1_write, message);
    send_message(ffs->fd_fifo_client_2_write, message);
}

void* receiveFleetThread(void* args) {
    FleetThreadArgs* fleetArgs = (FleetThreadArgs*)args;
    char buffer[BUFFER_SIZE_GRID];
    int fd_read = (fleetArgs->client_id == 1)
        ? fleetArgs->ffs->fd_fifo_client_1_read
        : fleetArgs->ffs->fd_fifo_client_2_read;
    int fd_write = (fleetArgs->client_id == 1)
        ? fleetArgs->ffs->fd_fifo_client_1_write
        : fleetArgs->ffs->fd_fifo_client_2_write;

    printf("Receiving fleet for Client %d...\n", fleetArgs->client_id);

    // precita cely buffer
    read(fd_read, buffer, sizeof(buffer));
    printf("Received fleet: %s\n", buffer);

    // rozbali lode z bufferu
    int x, y, isVertical, size;
    char* token = strtok(buffer, " ");
    while (token != NULL) {
        x = atoi(token);
        token = strtok(NULL, " ");
        y = atoi(token);
        token = strtok(NULL, " ");
        isVertical = atoi(token);
        token = strtok(NULL, " ");
        size = atoi(token);
        token = strtok(NULL, " ");

        // validuje a prida lod
        if (isPlacementValid(fleetArgs->fleetGrid, x, y, size, isVertical)) {
            placeShip(fleetArgs->fleetGrid, x, y, size, isVertical);
        } else {
            char message[BUFFER_SIZE];
            strcpy(message, "INVALID");
            send_message(fd_write, message);
            printf("Invalid fleet received from Client %d.\n", fleetArgs->client_id);
            return NULL;
        }
    }

    // ak vsetko prebehlo v poriadku 
    //TODO klinet z nejakeho dovodu prijme iba ""
    printf("Fleet received for Client %d.\n", fleetArgs->client_id);
    char message[BUFFER_SIZE];
    strcpy(message, "OK");
    send_message(fd_write, message);
    printf("Sending OK to Client %d.\n", fleetArgs->client_id);
    return NULL;
}




void run_server() {
fd_fifo_server_struct ffs = { -1, -1, -1, -1, -1, -1 };

init();

open_handshake(&ffs);

if (ffs.fd_fifo_handshake_read == -1 || ffs.fd_fifo_handshake_write == -1) {
  perror("Error can't open FIFO pipelines");
  exit(1);
}

  connect_clients(&ffs);

 //posle hracom spravu, ze mozu zacat vytvarat flotilu
  notifyClientsToStart(&ffs);

  //vytvorenie flotil pre server
  Grid fleetGrid1, fleetGrid2;
  initializeGrid(&fleetGrid1);
  initializeGrid(&fleetGrid2);
 
  //prijatie flotil od hracov
  receiveFleetParallel(&ffs, &fleetGrid1, &fleetGrid2);

  game(&ffs, &fleetGrid1, &fleetGrid2);

  // int num;
  // while (1) {
  //     // Čítanie od klienta 1 a posielanie klientovi 2
  //     if (read(ffs.fd_fifo_client_1_read, &num, sizeof(int)) > 0) {
  //         printf("Od klienta 1 prijaté číslo: %d\n", num);
  //         write(ffs.fd_fifo_client_2_write, &num, sizeof(int));
  //     }

  //     // Čítanie od klienta 2 a posielanie klientovi 1
  //     if (read(ffs.fd_fifo_client_2_read, &num, sizeof(int)) > 0) {
  //         printf("Od klienta 2 prijaté číslo: %d\n", num);
  //         write(ffs.fd_fifo_client_1_write, &num, sizeof(int));
  //     }
  // }

  close(&ffs);

  destroy();
}
