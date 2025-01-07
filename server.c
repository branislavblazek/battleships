#include "config.h"
#include "pipe.h"
#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef struct {
  int fd_fifo_handshake_read;
  int fd_fifo_handshake_write;
  int fd_fifo_client_1_read;
  int fd_fifo_client_1_write;
  int fd_fifo_client_2_read;
  int fd_fifo_client_2_write;
} fd_fifo_server_struct;

void init() {
  pipe_init(PIPE_HANDSHAKE_CLIENT_SERVER);
  pipe_init(PIPE_HANDSHAKE_SERVER_CLIENT);
  pipe_init(PIPE_CLIENT_1_SERVER);
  pipe_init(PIPE_SERVER_CLIENT_1);
  pipe_init(PIPE_CLIENT_2_SERVER);
  pipe_init(PIPE_SERVER_CLIENT_2);
}

void destroy() {
  pipe_destroy(PIPE_HANDSHAKE_CLIENT_SERVER);
  pipe_destroy(PIPE_HANDSHAKE_SERVER_CLIENT);
  pipe_destroy(PIPE_CLIENT_1_SERVER);
  pipe_destroy(PIPE_SERVER_CLIENT_1);
  pipe_destroy(PIPE_CLIENT_2_SERVER);
  pipe_destroy(PIPE_SERVER_CLIENT_2);
}

void open_handshake(fd_fifo_server_struct *ffs) {
  ffs->fd_fifo_handshake_read = pipe_open_read(PIPE_HANDSHAKE_CLIENT_SERVER);
  ffs->fd_fifo_handshake_write = pipe_open_write(PIPE_HANDSHAKE_SERVER_CLIENT);
}

void connect_clients(fd_fifo_server_struct *ffs) {
  int client_count = 0;

  while (client_count < 2) {
    char buffer[16];
    int bytes_read = read(ffs->fd_fifo_handshake_read, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      if (strcmp(buffer, "CONNECT") == 0) {
        client_count++;
        write(ffs->fd_fifo_handshake_write, &client_count, sizeof(client_count));

        if (client_count == 1) {
          ffs->fd_fifo_client_1_read = pipe_open_read(PIPE_CLIENT_1_SERVER);
          ffs->fd_fifo_client_1_write = pipe_open_write(PIPE_SERVER_CLIENT_1);
          printf("Client 1 connected.\n");
        } else if (client_count == 2) {
          ffs->fd_fifo_client_2_read = pipe_open_read(PIPE_CLIENT_2_SERVER);
          ffs->fd_fifo_client_2_write = pipe_open_write(PIPE_SERVER_CLIENT_2);
          printf("Client 2 connected.\n");
        }
      }
    }
  }
}

void close(fd_fifo_server_struct *ffs) {
  pipe_close(ffs->fd_fifo_client_1_read);
  pipe_close(ffs->fd_fifo_client_1_write);
  pipe_close(ffs->fd_fifo_client_2_read);
  pipe_close(ffs->fd_fifo_client_2_write);
  pipe_close(ffs->fd_fifo_handshake_read);
  pipe_close(ffs->fd_fifo_handshake_write);
}

void message_turn(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE];
  strcpy(message, "TURN");

  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, message);
}

void message_wait(fd_fifo_server_struct *ffs, int turn) {
  char message[BUFFER_SIZE];
  strcpy(message, "WAIT");
  
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, message);
}

void message_bye(fd_fifo_server_struct *ffs) {
  char message[BUFFER_SIZE];
  strcpy(message, "BYE");
  
  send_message(ffs->fd_fifo_client_1_write, message);
  send_message(ffs->fd_fifo_client_2_write, message);
}

int read_turn(fd_fifo_server_struct *ffs, int turn, char *coords) {
  char buffer[BUFFER_SIZE];

  int fd = turn == 0 
    ? ffs->fd_fifo_client_1_read
    : ffs->fd_fifo_client_2_read;

  read_message(fd, buffer);

  if (buffer[0] == '\0') {
    return 0;
  }

  strncpy(coords, buffer, 2);
  coords[2] = '\0';
  return 1;
}

void message_turn_data(fd_fifo_server_struct *ffs, int turn, char *data) {
  int fd = turn == 0
    ? ffs->fd_fifo_client_1_write
    : ffs->fd_fifo_client_2_write;
  
  send_message(fd, data);
}

void game(fd_fifo_server_struct *ffs) {
  char coords[3];
  // char message[BUFFER_SIZE];
  int turn = 0;
  // will be removed....
  int MAX_ATTEMPS = 8;
  int attemps= 0; 

  while (attemps < MAX_ATTEMPS) {
    message_turn(ffs, turn);
    message_wait(ffs, 1 - turn);

    int ok = read_turn(ffs, turn, coords);
    if (!ok) {
      continue;
    }

    printf("Got _%s_ from client %d\n", coords, turn + 1);

    //snprintf(message, "EN%s", coords);
    // message_turn_data(ffs, 1 - turn, message);
    

    turn = 1 - turn;
    attemps++;
  }

  message_bye(ffs);
}

void run_server() {
fd_fifo_server_struct ffs = { -1, -1, -1, -1, -1, -1 };

init();

open_handshake(&ffs);

if (ffs.fd_fifo_handshake_read == -1 || ffs.fd_fifo_handshake_write == -1) {
  perror("Error can't open FIFO pipelines");
  exit(1);
}

connect_clients(&ffs);

game(&ffs);

  // int num;
  // while (1) {
  //     // Čítanie od klienta 1 a posielanie klientovi 2
  //     if (read(ffs.fd_fifo_client_1_read, &num, sizeof(int)) > 0) {
  //         printf("Od klienta 1 prijaté číslo: %d\n", num);
  //         write(ffs.fd_fifo_client_2_write, &num, sizeof(int));
  //     }

  //     // Čítanie od klienta 2 a posielanie klientovi 1
  //     if (read(ffs.fd_fifo_client_2_read, &num, sizeof(int)) > 0) {
  //         printf("Od klienta 2 prijaté číslo: %d\n", num);
  //         write(ffs.fd_fifo_client_1_write, &num, sizeof(int));
  //     }
  // }

  close(&ffs);

  destroy();
}
