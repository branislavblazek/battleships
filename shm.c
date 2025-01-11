#include "shm.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int shm_init(char *shm_name) {
  int fd_shm = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

  if (fd_shm == -1) {
    perror("Failed to create shared memory");
  }

  return fd_shm;
}

int shm_init_read(char *shm_name) {
  const int fd_shm = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);

  if (fd_shm == -1) {
    perror("Failed to open existing shared memory");
  }

  return fd_shm;
}

void shm_destroy(char *shm_name) {
  if (shm_unlink(shm_name) == -1) {
    perror("Failed to unlink shared memory");
    exit(EXIT_FAILURE);
  }
}


void shm_set_size(char *shm_name, int fd_shm, off_t size) {
  if (ftruncate(fd_shm, size) == -1) {
    perror("Failed to truncate shared memory");
    shm_unlink(shm_name);
    exit(EXIT_FAILURE);
  }
}


void* shm_map(int fd_shm, off_t size){
  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
}

void shm_unmap(int *ptr, off_t size) {
  munmap(ptr, size);
}
