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

void connect_client(int* connected, int* fd_read, int*fd_write) {
  if (!*connected) {
    // TODO: here continue
  }
}

void connect_clients(fd_pipes_struct* fd_pipes) {

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

  fd_pipes.fd_pipe_server = pipe_open_write(PIPE_SERVER);

  printf("Server waits for clients...\n");

  //while (1) {
    connect_clients(&fd_pipes);
  //}

/*
  int id;
  char buffer[3];
  read(fd_pipe, &id, sizeof(int));
  read(fd_pipe, buffer, sizeof(buffer));
  */

  close(&fd_pipes);

  pipe_destroy(PIPE_SERVER);
}
