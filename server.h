#pragma once
#include "grid.h"
void deserializeGrid(Grid *grid, const char *buffer);
void receiveGridFromClient(int fd_read, Grid *grid);
void run_server( ); 