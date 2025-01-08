#pragma once
#include "player.h"

void createAndSendFleet(int fd_read, int fd_write, Player* player);

void serializeFleet(Player* player, char* buffer, int shipsCount);

void clearInputBuffer();

void run_client();