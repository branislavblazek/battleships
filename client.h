#pragma once

#include "config.h"
#include "pipe.h"
#include "communication.h"
#include "player.h"
#include "client.h"
#include "utils.h"
#include "interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define SHIP_SIZES {1}
#define SHIPS_COUNT 1

typedef struct {
    int fd_fifo_handshake_read;
    int fd_fifo_handshake_write;
    int fd_fifo_server_write;
    int fd_fifo_server_read;
} fd_fifo_client_struct;

void open_client_handshake(fd_fifo_client_struct* ffc);
void connect_server(fd_fifo_client_struct* ffc);
void close_client_pipes(fd_fifo_client_struct* ffc);

void waitForStart(fd_fifo_client_struct* ffc);
void game(fd_fifo_client_struct* ffc, Player* player);

void process_turn(fd_fifo_client_struct* ffc, char* buffer, Player* player);
int processNeighbours(char* buffer, Grid* grid);

void sendFleet(int fd_read, int fd_write, Player* player);
void createAndSendFleet(int fd_read, int fd_write, Player* player);
void generateFleet(int fd_read, int fd_write, Player* player);
void serializeFleet(Player* player, char* buffer, int shipsCount);

void* threadRendering(void* arg);

void run_client();