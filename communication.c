#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int read_message(int fd, char *buffer) {
  int bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
  if (bytes_read == 0) {
    puts("Connection was interrupted!");
    return EXIT_FAILURE;
  }

  buffer[bytes_read] = '\0';

  return EXIT_SUCCESS;
}

void send_message(int fd, char *buffer) {
  write(fd, buffer, BUFFER_SIZE);
}