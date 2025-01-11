#ifndef SERVER_H
#define SERVER_H

#pragma once

#include "grid.h"
#include "config.h"
#include "pipe.h"
#include "communication.h"
#include "player.h"
#include "utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

typedef struct {
	int fd_fifo_handshake_read;
	int fd_fifo_handshake_write;
	int fd_fifo_client_1_read;
	int fd_fifo_client_1_write;
	int fd_fifo_client_2_read;
	int fd_fifo_client_2_write;
} fd_fifo_server_struct;

typedef struct {
	fd_fifo_server_struct* ffs;
	int client_id;
	Grid* fleetGrid;
} FleetThreadArgs;

void init();
void destroy();
void open_server_handshake(fd_fifo_server_struct* ffs);
void connect_clients(fd_fifo_server_struct* ffs);
void close_server_pipes(fd_fifo_server_struct* ffs);

void messageTurn(fd_fifo_server_struct* ffs, int turn);
void messageWait(fd_fifo_server_struct* ffs, int turn);
void messageByeAll(fd_fifo_server_struct* ffs);
void messageBye(fd_fifo_server_struct* ffs, int turn);
void messageWin(fd_fifo_server_struct* ffs, int turn);
void messageLost(fd_fifo_server_struct* ffs, int turn);
void messageTurnData(fd_fifo_server_struct* ffs, int turn, char* data);
void messageStartAll(fd_fifo_server_struct* ffs);

int read_turn(fd_fifo_server_struct* ffs, int turn, char* coords);
int makeShoot(char* data, Grid* fleetGrid1, Grid* fleetGrid2, int client);
int checkWon(Grid* fleetGrid1, Grid* fleetGrid2, int wasHit);
int checkSink(Grid* fleetGrid, char* coords, char* buffer);

void game_server(fd_fifo_server_struct* ffs, Grid* fleetGrid1, Grid* fleetGrid2);

void receiveFleetParallel(fd_fifo_server_struct* ffs, Grid* grid1, Grid* grid2);
void* receiveFleetThread(void* args);

void run_server();

#endif // SERVER_H