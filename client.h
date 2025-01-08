#pragma once
#include "player.h"
void createAndSendFleet(int fd_read, int fd_write, Player* player);
void serializeGrid(Grid *grid, char *buffer);
void sendGridToServer(int fd_write, Grid *grid);
void run_client();