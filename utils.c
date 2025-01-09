#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int parseInput(const char *input, int *x, int *y) {
  char letter = '\0';
  char number = '\0';

  if (isalpha(input[0])) {
    letter = input[0];
    number = input[1];
  } else if (isalpha(input[1])) {
    letter = input[1];
    number = input[0];
  } else {
    printf("Invalid input. Please enter a letter and a number (e.g., A1 or 1A).\n");
    return 0;
  }

  if (toupper(letter) >= 'A' && toupper(letter) <= 'J') {
    *x = toupper(letter) - 'A'; // A -> 0, ..., J -> 9
  } else {
    printf("Invalid row. Please enter a letter between A-J.\n");
    return 0;
  }

  if (isdigit(number) && number >= '0' && number <= '9') {
    *y = number - '0'; // '0' -> 0, ..., '9' -> 9
  } else {
    printf("Invalid column. Please enter a number between 0-9.\n");
    return 0;
  }

  return 1;
}