#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Pre usleep()
#include <string.h> // Pre strcmp()
#include <time.h>   // Pre srand()
#include "grid.h"

#define RESET "\033[0m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define CLEAR_SCREEN "\033[2J"


#define TERMINAL_WIDTH 60
#define TERMINAL_HEIGHT 30

void clearScreen() {
    printf(CLEAR_SCREEN);
    printf("\033[H");
    fflush(stdout);
}

void printSimpleShip() {
    
    const char* ship[] = {
        "                                      )___(",
        "                               _______/__/_",
        "                      ___     /===========|   ___",
        "     ____       __   [\\]___/____________|__[//]   __",
        "     \\   \\_____[\\]__/___________________________\\__[//]___",
        "      \\                                                    |",
        "       \\                                                  /",
        "    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
    };
    printf("\n");
    int shipHeight = sizeof(ship) / sizeof(ship[0]);
    for (int i = 0; i < shipHeight; i++) {
        
        printf("%s\n", ship[i]);
    }
    printf("\n");
}

void printCenteredShip() {
    
    const char* ship[] = {
        "                                  # #  ( )",
        "                               ___#_#___|__",
        "                           _  |____________|  _",
        "                    _=====| | |            | | |==== _",
        "              =====| |.---------------------------. | |====",
        "<--------------------'   .  .  .  .  .  .  .  .   '--------------/",
        "  \\                                                             /",
        "   \\___________________________________________________________/",
        "  wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
        "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
        "   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww ",
        "\n"
    };
     clearScreen();

    int shipHeight = sizeof(ship) / sizeof(ship[0]);
    for (int i = 0; i < shipHeight; i++) {
        printf("%s\n", ship[i]);
    }
}

void centerText(const char* text, const char* color) {
    int padding = (TERMINAL_WIDTH - strlen(text)) / 2;
    printf("%s%*s%s%s\n", color, padding, "", text, RESET);
}

void printBanner() {
    clearScreen();
    for (int i = 0; i < (TERMINAL_HEIGHT / 2 - 4); i++) printf("\n");
    centerText("==========================================", CYAN);
    centerText("WELCOME TO BATTLESHIPS", CYAN);
    centerText("==========================================", CYAN);
    for (int i = 0; i < 2; i++) printf("\n");
    centerText("Prepare for an epic battle on the seas...", GREEN);
    printf("\n");
    centerText("Waiting for the second player to join...", "");
    printf("\n");
    centerText("Tip: Resize your terminal to at least 30x100 for the best experience!", RED);
    for (int i = 0; i < (TERMINAL_HEIGHT / 2 - 4); i++) printf("\n");
}

void waitingAnimation() {
    const char *dots = ".   ";
    for (int i = 0; i < 10; i++) {
        printf("\r%-100s", "");
        printf("\r");
        centerText("Waiting", "");
        printf("%s", dots + (i % 4)); 
        fflush(stdout); 
        usleep(500000); 
    }
    printf("\n");
    centerText("Second player has joined!", GREEN);
}

void chooseFleetOption() {
    clearScreen();
    for (int i = 0; i < (TERMINAL_HEIGHT / 2 - 4); i++) printf("\n");
    centerText("==========================================", GREEN);
    centerText("CHOOSE YOUR FLEET SETUP", GREEN);
    centerText("==========================================", GREEN);
    for (int i = 0; i < 2; i++) printf("\n");
    centerText("1. Manual setup (You place your ships)", "");
    centerText("2. Random setup (Ships placed automatically)", "");
    printf("\n\n\n\n\n\n\n");
    for (int i = 0; i < TERMINAL_WIDTH; i++) printf("-");
    printf("\n");
}

void printCenteredGrid(Grid* grid) {
    
    const int DRAWING_HEIGHT = 25;

    int horizontalPadding = (TERMINAL_WIDTH - (GRID_SIZE * 4 + 6)) / 2 + 4; 

    clearScreen();

    int verticalPadding = (DRAWING_HEIGHT - GRID_SIZE - 3) / 2 / 2; 
    for (int i = 0; i < verticalPadding; i++) {
        printf("\n");
    }
     centerText("===========================================", CYAN);
    centerText("CREATE YOUR FLEET", CYAN);
    centerText("===========================================", CYAN);
    
    printf("%*s", horizontalPadding, ""); 
    printf(CYAN "+");
    for (int i = 0; i < GRID_SIZE * 4 - 5; i++) printf("-");
    printf("+\n" RESET);

    printf("%*s", horizontalPadding, ""); 
    printf(CYAN "|    "); 
    for (int j = 0; j < GRID_SIZE; j++) {
        printf(YELLOW "[%d]" RESET, j);
    }
    printf(CYAN " |\n" RESET);

    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%*s", horizontalPadding, ""); 
        printf(CYAN "| " RESET);
        printf(YELLOW "[%c]" RESET, 'A' + i);

        for (int j = 0; j < GRID_SIZE; j++) {
            char* cellColor = RESET;
            char cellSymbol = '~'; 
            switch (grid->cells[i][j]) {
                case EMPTY:
                    cellSymbol = '~';
                    cellColor = BLUE;
                    break;
                case SHIP:
                    cellSymbol = '#';
                    cellColor = GREEN;
                    break;
                case HIT:
                    cellSymbol = 'X';
                    cellColor = RED;
                    break;
                case MISS:
                    cellSymbol = 'O';
                    cellColor = YELLOW;
                    break;
                default:
                    break;
            }

            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }
        printf(CYAN " |\n" RESET);
    }

    printf("%*s", horizontalPadding, ""); // Odsadenie
    printf(CYAN "+");
    for (int i = 0; i < GRID_SIZE * 4 - 5; i++) printf("-");
    printf("+\n" RESET);

    for (int i = 0; i < verticalPadding; i++) {
        printf("\n");
    }
    for (int i = 0; i < TERMINAL_WIDTH; i++) {
        printf("-");
    }
    printf("\n");
}

void printDoubleGrid(Grid *grid1, Grid *grid2) {
    const int DRAWING_HEIGHT = 25; 

    clearScreen();

    int verticalPadding = (DRAWING_HEIGHT - 10 - 3) / 2; // 10 riadkov pre gridy + 3 riadky pre nadpisy
    for (int i = 0; i < verticalPadding; i++) {
        printf("\n");
    }

    printf("%*s" YELLOW "Tracking Grid" RESET "                          " YELLOW "Your Grid\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");

    printf("%*s   ", (TERMINAL_WIDTH - 72) / 2, "");
    for (int j = 0; j < 10; j++) printf(YELLOW "[%d]" RESET, j);
    printf("         ");
    for (int j = 0; j < 10; j++) printf(YELLOW "[%d]" RESET, j);
    printf("\n");

    for (int i = 0; i < 10; i++) {
        printf("%*s" YELLOW "[%c]" RESET, (TERMINAL_WIDTH - 72) / 2, "", 'A' + i);
        
        for (int j = 0; j < 10; j++) {
            char* cellColor = RESET;
            char cellSymbol = '~'; 
            switch (grid1->cells[i][j]) {
                case EMPTY: cellSymbol = '~'; cellColor = BLUE; break;
                case SHIP: cellSymbol = '#'; cellColor = GREEN; break;
                case HIT: cellSymbol = 'X'; cellColor = RED; break;
                case MISS: cellSymbol = 'O'; cellColor = YELLOW; break;
                default: break;
            }
            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }

        printf("      "); 

        printf(YELLOW "[%c]" RESET, 'A' + i);
        for (int j = 0; j < 10; j++) {
            char* cellColor = RESET;
            char cellSymbol = '~'; 
            switch (grid2->cells[i][j]) {
                case EMPTY: cellSymbol = '~'; cellColor = BLUE; break;
                case SHIP: cellSymbol = '#'; cellColor = GREEN; break;
                case HIT: cellSymbol = 'X'; cellColor = RED; break;
                case MISS: cellSymbol = 'O'; cellColor = CYAN; break;
                default: break;
            }
            printf("%s[%c]" RESET, cellColor, cellSymbol);
        }

        printf("\n");
    }

    printf("\n%*s" GREEN "Indicator:" RESET "                             " GREEN "Indicator:\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");
    printf("%*s" GREEN "Carrier [5]" RESET "                            " GREEN "Carrier [5]\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");
    printf("%*s" GREEN "Battleship [4]" RESET "                         " GREEN "Battleship [4]\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");
    printf("%*s" GREEN "Destroyer [3]" RESET "                          " GREEN "Destroyer [3]\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");
    printf("%*s" GREEN "Submarine [3]" RESET "                          " GREEN "Submarine [3]\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");
    printf("%*s" GREEN "PatrolBoat [2]" RESET "                         " GREEN "PatrolBoat [2]\n" RESET, (TERMINAL_WIDTH - 72) / 2, "");

    int remainingPadding = DRAWING_HEIGHT - verticalPadding - 10 - 6; 
    for (int i = 0; i < remainingPadding; i++) {
        printf("\n");
    }

    for (int i = 0; i < TERMINAL_WIDTH; i++) {
        printf("-");
    }
    printf("\n");
}

void printEndScreen(int won) {
    clearScreen();
    
    for (int i = 0; i < 20; i++) {
        printf("\n");
    }

    const char *message = won ? "YOU WON" : "YOU LOST";
    const char *color = won ? GREEN : RED;
    int messageLength = strlen(message);

    int boxWidth = messageLength + 8; // Text + priestor okolo
    int boxPadding = (TERMINAL_WIDTH - boxWidth) / 2;

    printf("%*s", boxPadding, ""); // Odsadenie
    printf(CYAN "+");
    for (int i = 0; i < boxWidth; i++) printf("-");
    printf("+\n");

    printf("%*s", boxPadding, ""); // Odsadenie
    printf(CYAN "|    " RESET "%s%s%s    " CYAN "|\n" RESET, color, message, RESET);

    printf("%*s", boxPadding, ""); // Odsadenie
    printf(CYAN "+");
    for (int i = 0; i < boxWidth; i++) printf("-");
    printf("+\n");

    printf("\n\n");

    printSimpleShip();

    for (int i = 0; i < 5; i++) {
        printf("\n");
    }
      printf("\033[%d;1H", TERMINAL_HEIGHT - 2);
}



