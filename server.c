#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void run_server() {
  int fd_pipe = pipe_open_read(PIPE_PATH);

  int id;
  char buffer[3];
  read(fd_pipe, &id, sizeof(int));
  read(fd_pipe, buffer, strlen(buffer));
  
  pipe_close(fd_pipe);

  buffer[3] = '\0';
  printf("got user id %d with message %s.\n", id, buffer);  
}
