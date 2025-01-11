#include "server.h"


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

void open_server_handshake(fd_fifo_server_struct *ffs) {
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

void close_server_pipes(fd_fifo_server_struct *ffs) {
  pipe_close(ffs->fd_fifo_client_1_read);
  pipe_close(ffs->fd_fifo_client_1_write);
  pipe_close(ffs->fd_fifo_client_2_read);
  pipe_close(ffs->fd_fifo_client_2_write);
  pipe_close(ffs->fd_fifo_handshake_read);
  pipe_close(ffs->fd_fifo_handshake_write);
}

void messageTurn(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE] = "TURN";

  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  sendMessage(fd, message);
}

void messageWait(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE] = "WAIT";
  
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  sendMessage(fd, message);
}

void messageByeAll(fd_fifo_server_struct *ffs) {
  char message[BUFFER_SIZE] = "BYE";
  
  sendMessage(ffs->fd_fifo_client_1_write, message);
  sendMessage(ffs->fd_fifo_client_2_write, message);
}

void messageBye(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE] = "BYE";
  
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  sendMessage(fd, message);
}

void messageWin(fd_fifo_server_struct *ffs, int turn) {
    char message[BUFFER_SIZE] = "WIN";
    int fd = turn == 0
        ? ffs->fd_fifo_client_1_write
        : ffs->fd_fifo_client_2_write;

    printf("Sending WIN to Client %d (FIFO descriptor: %d)\n", turn + 1, fd);
    sendMessage(fd, message);
}

void messageLost(fd_fifo_server_struct *ffs, int turn) {
    char message[BUFFER_SIZE] = "LOST";
    int fd = turn == 0
        ? ffs->fd_fifo_client_1_write
        : ffs->fd_fifo_client_2_write;

    printf("Sending LOST to Client %d (FIFO descriptor: %d)\n", turn + 1, fd);
    sendMessage(fd, message);
}

void messageTurnData(fd_fifo_server_struct *ffs, int turn, char *data) {
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  sendMessage(fd, data);
}

void messageStartAll(fd_fifo_server_struct *ffs) {
  char message[BUFFER_SIZE] = "START";

  sendMessage(ffs->fd_fifo_client_1_write, message);
  sendMessage(ffs->fd_fifo_client_2_write, message);
}

int read_turn(fd_fifo_server_struct *ffs, int turn, char *coords) {
  char buffer[BUFFER_SIZE];

  int fd = turn == 0 
    ? ffs->fd_fifo_client_1_read
    : ffs->fd_fifo_client_2_read;

  int ok = readMessage(fd, buffer);

  if (ok == 1) { // disconnected
    return -1;
  }

  if (buffer[0] == '\0') {
    return 0;
  }
   
  strncpy(coords, buffer, 2);
  coords[2] = '\0';
  return 1;
}

int makeShoot(char* data, Grid *fleetGrid1, Grid *fleetGrid2, int client) {
  char coordsData[3];
  int x, y, current;

  coordsData[0] = data[0];
  coordsData[1] = data[1];
  coordsData[2] = '\0';

  int ok = parseInput(coordsData, &x, &y);
  if (!ok) {
    printf("Can't decode coords from server: %s\n", coordsData);
    return 0;
  }

  if (client == 1) {
    current = fleetGrid2->cells[x][y];
    if (current == SHIP || current == EMPTY) {
      fleetGrid2->cells[x][y] = current == SHIP ? HIT : MISS;
    }
    return current;
  } else if (client == 2) {
    current = fleetGrid1->cells[x][y];
    if (current == SHIP || current == EMPTY) {
      fleetGrid1->cells[x][y] = current == SHIP ? HIT : MISS;
    }
    return current;
  }

  return EMPTY;
}

int checkWon(Grid *fleetGrid1, Grid *fleetGrid2, int wasHit) {
  if (!wasHit) return 0;

  int client1ships = checkShips(fleetGrid1);
  int client2ships = checkShips(fleetGrid2);

  if (client1ships == 0) {
    return 2;
  } else if (client2ships == 0) {
    return 1;
  }

  return 0;
}

int checkSink(Grid *fleetGrid, char *coords, char* buffer) {
  int x, y;

  int ok = parseInput(coords, &x, &y);
  if (!ok) {
    printf("Can't decode coords from server: %s\n", coords);
    return 0;
  }

  int sink = isShipSink(fleetGrid, x, y);
  if (sink) getMissAroundSinkShip(fleetGrid, x, y, buffer);

  return sink;
}

void game_server(fd_fifo_server_struct *ffs, Grid *fleetGrid1, Grid *fleetGrid2) {
  char coords[3];
  char target_message[BUFFER_SIZE];
  char neighbour_buffer[100];
  int turn = 0;
  int smooth_run = 1;

  memset(neighbour_buffer, 0, sizeof(neighbour_buffer));

  while (1) {
    usleep(100000); // valgrind
    
    messageTurn(ffs, turn);
    messageWait(ffs, 1 - turn);
    
    usleep(100000); // valgrind

    int ok = read_turn(ffs, turn, coords);
    if (ok == 0) {
      continue;
    } else if (ok == -1) {
      messageBye(ffs, 1 - turn);
      smooth_run = 0;
      puts("Exit due to client disconnected.");
      break;
    }

    int shoot = makeShoot(coords, fleetGrid1, fleetGrid2, turn + 1);
    int hit = shoot == SHIP;
    int won = checkWon(fleetGrid1, fleetGrid2, hit);

    if (won > 0) {
      printf("Game won by Player %d (turn: %d)\n", won, turn);
      printf("Descriptors: Client 1 = %d, Client 2 = %d\n",
           ffs->fd_fifo_client_1_write, ffs->fd_fifo_client_2_write);

       //usleep(10000); // valgrind	

      messageWin(ffs, won - 1); //oprava, malo by to uz fungovat
      messageLost(ffs, 2 - won);

      usleep(1000);

      messageByeAll(ffs);
      break;
    }

    int sink = 0;

    if (hit) {
      sink = checkSink(turn == 1 ? fleetGrid1 : fleetGrid2, coords, neighbour_buffer);
    }

    char target = (shoot == HIT || shoot == SHIP) ? 'X' : '.';
    char isSink = sink ? 'Y' : 'N';
    sprintf(target_message, "XY%s%c_%c", coords, target, isSink);
    target_message[7] = '\0';

    //usleep(100000); // valgrind

    target_message[5] = 'M';
    messageTurnData(ffs, turn, target_message);

    target_message[5] = 'E';
    messageTurnData(ffs, 1 - turn, target_message);

    //usleep(100000); // valgrind

    usleep(1000);

    if (hit && sink) {
      write(ffs->fd_fifo_client_1_write, neighbour_buffer, 100);
      write(ffs->fd_fifo_client_2_write, neighbour_buffer, 100);
      memset(neighbour_buffer, 0, sizeof(neighbour_buffer));
    }

    usleep(1000);

    turn = 1 - turn;
  }

  if (smooth_run) messageByeAll(ffs);
}

void receiveFleetParallel(fd_fifo_server_struct* ffs, Grid* grid1, Grid* grid2) {
  printf("starting receiveFleetParallel");

  pthread_t thread1, thread2;
  FleetThreadArgs args1 = { ffs, 1, grid1 };
  FleetThreadArgs args2 = { ffs, 2, grid2 };

  // dve paralelne vlakna na zvolenie flotily
  pthread_create(&thread1, NULL, receiveFleetThread, &args1);
  pthread_create(&thread2, NULL, receiveFleetThread, &args2);

  // synchornizacia vlakien
  //bez tychto vypisov to nefunguje, mozno treba nejaky usleep
  int fleet1_valid = pthread_join(thread1, NULL);
  printf("Fleet for Client 1 received successfully: %s\n", fleet1_valid == 0 ? "YES" : "NO");

  int fleet2_valid = pthread_join(thread2, NULL);
  printf("Fleet for Client 2 received successfully: %s\n", fleet2_valid == 0 ? "YES" : "NO");

if (fleet1_valid != 0 || fleet2_valid != 0) {
    printf("Error: One or both fleets are invalid. Terminating server.\n");
    exit(1);
}
  printf("Both fleets have been received.\n");
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
      int shipPlaced = placeShip(fleetArgs->fleetGrid, x, y, size, isVertical);
      if(shipPlaced) {
        //TODO vymazat
      } else {
          char message[BUFFER_SIZE];
          strcpy(message, "INVALID");
          sendMessage(fd_write, message);
          printf("Invalid fleet received from Client %d.\n", fleetArgs->client_id);
          return NULL;
      }
  }
  char message[BUFFER_SIZE] = "FLEET_OK";
  sendMessage(fd_write, message);
  return NULL;
}

void run_server() {
  fd_fifo_server_struct ffs = { -1, -1, -1, -1, -1, -1 };

  // destroy();

  signal(SIGINT, destroy);
  atexit(destroy);

  init();
  open_server_handshake(&ffs);

  if (ffs.fd_fifo_handshake_read == -1 || ffs.fd_fifo_handshake_write == -1) {
    perror("Error can't open FIFO pipelines");
    exit(1);
  }

  connect_clients(&ffs);

  messageStartAll(&ffs);

  Grid fleetGrid1, fleetGrid2;
  initializeGrid(&fleetGrid1);
  initializeGrid(&fleetGrid2);
 
  receiveFleetParallel(&ffs, &fleetGrid1, &fleetGrid2);

  game_server(&ffs, &fleetGrid1, &fleetGrid2);
  close_server_pipes(&ffs);

  destroy();
}
