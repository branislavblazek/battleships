#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void pipe_destroy(const char *path) {
  if (unlink(path) == -1) {
    if (errno == ENOENT) {
      return;
    }
    perror("Failed to unlink named pipe");
  }
}

void pipe_init(const char *path) {
  pipe_destroy(path);

  if (mkfifo(path, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP) == -1) {
    perror("Failed to create named pipe");
    exit(EXIT_FAILURE);
  }
}

static int open_pipe(const char *path, int flags) {
  const int fd = open(path, flags);
  if (fd == -1) {
    fprintf(stderr, "Failed to open named pipe %s", path);
    perror("");
    exit(EXIT_FAILURE);
  }
  return fd;
}

int pipe_open_write(const char *path) {
  return open_pipe(path, O_WRONLY);
}

int pipe_open_read(const char *path) {
  return open_pipe(path, O_RDONLY);
}

void pipe_close(const int fd) {
  if (close(fd) == -1) {
    perror("Failed to close pipe");
    exit(EXIT_FAILURE);
  }
}

