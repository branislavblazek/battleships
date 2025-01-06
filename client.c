#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int connect_server(int* fd_read, int* fd_write, const char* pipe_path) {
  printf("gooo\n");
  *fd_read = pipe_open_read(pipe_path);
  *fd_write = pipe_open_write(pipe_path);
  printf("%d %d", *fd_read, *fd_write);  
  if (*fd_read >= 0 && *fd_write >= 0) {
    return 1;
  }

  return 0;
}

void run_client() {
  puts("Running client...");
  int fd_pipe_read, fd_pipe_write;
  int client_id, is_connected;

  fd_pipe_read = pipe_open_read(PIPE_CLIENT_1);
  fd_pipe_write = pipe_open_write(PIPE_CLIENT_1);
  //is_connected = connect_server(&fd_pipe_read, &fd_pipe_write, PIPE_CLIENT_1);
  //printf("%d\n", is_connected);
  printf("%d %d\n", fd_pipe_read, fd_pipe_write);
  return;
  if (is_connected == 1) {
    client_id = 1;
  } else {
    is_connected = connect_server(&fd_pipe_read, &fd_pipe_write, PIPE_CLIENT_2);
    
    if (is_connected == 1) {
      client_id = 2;
    } else {  
      printf("Server is full!\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("Current client is #%d\n", client_id);
}
