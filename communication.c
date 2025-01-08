#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void read_message(int fd, char *buffer) {
  int bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
  buffer[bytes_read] = '\0';
}

void send_message(int fd, char *buffer) {
  write(fd, buffer, BUFFER_SIZE);
}