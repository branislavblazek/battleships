#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void run_client() {
  int fd_pipe = pipe_open_write(PIPE_PATH);
  sleep(2);
  int id = 12;
  char message[3] = "A2";
  printf("Client sends message\n");
  write(fd_pipe, &id, sizeof(int));
  write(fd_pipe, message, sizeof(message));

  pipe_close(fd_pipe);
}
