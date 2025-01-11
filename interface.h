#pragma once

#define RESET           "\033[0m"
#define BLUE            "\033[34m"
#define GREEN           "\033[32m"
#define RED             "\033[31m"
#define YELLOW          "\033[33m"
#define CYAN            "\033[36m"
#define CLEAR_SCREEN    "\033[2J"
#define RESET_COLOR     "\033[0m"


void clearScreen();
void printBanner();
void waitingAnimation();
void printCenteredGrid();
int chooseFleetOption();
void printDoubleGrid();
void printEndScreen(int won);
