#include "server.h"
#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  int client_1_connected, client_2_connected;
  int fd_pipe_server;
  int fd_pipe_client_1_read, fd_pipe_client_1_write;
  int fd_pipe_client_2_read, fd_pipe_client_2_write;
} fd_pipes_struct;

void init() {
  pipe_init(PIPE_SERVER);
  pipe_init(PIPE_CLIENT_1);
  pipe_init(PIPE_CLIENT_2);
}

void connect_client(int* connected, int* fd_read, int*fd_write, const char* pipe_path) {
  if (!*connected) {
    puts("now trying to connect client");
    *fd_read = pipe_open_read(pipe_path);
    *fd_write = pipe_open_write(pipe_path);
    
    if (*fd_read >= 0 && *fd_write >= 0) {
      *connected = 1;
      printf("Successfully connected client with PIPE PATH = %s\n", pipe_path);
    }
  }
}

void connect_clients(fd_pipes_struct* fd_pipes) {
  connect_client(
    &fd_pipes->client_1_connected, 
    &fd_pipes->fd_pipe_client_1_read,
    &fd_pipes->fd_pipe_client_1_write,
    PIPE_CLIENT_1);
  
  connect_client(
    &fd_pipes->client_2_connected, 
    &fd_pipes->fd_pipe_client_2_read,
    &fd_pipes->fd_pipe_client_2_write,
    PIPE_CLIENT_2);
} 

void close(fd_pipes_struct* fd_pipes) {
  pipe_close(fd_pipes->fd_pipe_server);
  pipe_close(fd_pipes->fd_pipe_client_1_read);
  pipe_close(fd_pipes->fd_pipe_client_1_write);
  pipe_close(fd_pipes->fd_pipe_client_2_read);
  pipe_close(fd_pipes->fd_pipe_client_2_write);
}

void run_server() {
  fd_pipes_struct fd_pipes;
  
  init();  
  
  //fd_pipes.fd_pipe_server = pipe_open_write(PIPE_SERVER);

  while (1) {
    connect_clients(&fd_pipes);
  }

  close(&fd_pipes);

  pipe_destroy(PIPE_SERVER);
}
