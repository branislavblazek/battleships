#include "server.h"
#include "client.h"
#include "pipe.h"
#include "config.h"
#include "player.h"
#include "shm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>

int main() {
    int fd_shm, isNewShm = 0;

    fd_shm = shm_init(SERVER_FORK_SHM);
    if (fd_shm == - 1) {
      if (errno == EEXIST) {
        fd_shm = shm_init_read(SERVER_FORK_SHM);

        if (fd_shm == -1) {
          perror("Failed to open existing shared memory");
          exit(1);
        }
      } else {
        perror("Failed to create shared memory");
        exit(1);
      }
    } else {
      isNewShm = 1;
    }

    if (isNewShm) shm_set_size(SERVER_FORK_SHM, fd_shm, sizeof(int));

    int *isServerRunningPtr = shm_map(fd_shm, sizeof(int));

    if (isServerRunningPtr == MAP_FAILED) {
      perror("Failed to map shared memory");
      if (isNewShm) shm_unlink(SERVER_FORK_SHM);
      exit(1);
    }

    if (isNewShm) *isServerRunningPtr = 0;

    if (*isServerRunningPtr == 0) {
      pid_t server_pid = fork();

      if (server_pid == -1) {
        perror("Failed to create server process");
        shm_unmap(isServerRunningPtr, sizeof(int));
        shm_unlink(SERVER_FORK_SHM);
        exit(1);
      }

      if (server_pid == 0) {
        puts("START SERVER");
        *isServerRunningPtr = 1;
        run_server();
        puts("END SERVER");
        *isServerRunningPtr = 0;
        exit(0);
      }
    } else {
      puts("Server is already running. Starting client...");
    }

    run_client();
    wait(NULL);

    shm_unmap(isServerRunningPtr, sizeof(int));

    close(fd_shm);

    if (isNewShm) shm_unlink(SERVER_FORK_SHM);

    return 0;
}
