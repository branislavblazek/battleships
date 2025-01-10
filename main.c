#include "server.h"
#include "client.h"
#include "pipe.h"
#include "config.h"
#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {
  if (argc == 2 && strcmp(argv[1], "server") == 0) {
    pid_t server_pid = fork();
    if (server_pid == -1) {
        perror("Failed to create server process");
        exit(1);
    }

    if (server_pid == 0) {
        puts("START SERVER");
        run_server();
        puts("END SERVER");
        exit(0);
    }
  }

  run_client();
  wait(NULL);

  return 0;
}
