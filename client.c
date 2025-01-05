#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void run_client() {
  int fd_pipe = pipe_open_write(PIPE_PATH);
  sleep(3);
  int id = 1;
  char message[3] = "A2";
  write(fd_pipe, &id, sizeof(int));
  write(fd_pipe, message, strlen(message));

  pipe_close(fd_pipe);
}
