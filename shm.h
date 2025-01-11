#pragma once

#include <sys/types.h>

int shm_init(char *shm_name);

int shm_init_read(char *shm_name);

void shm_destroy(char *shm_name);

void shm_set_size(char *shm_name, int fd_shm, off_t size);

void* shm_map(int fd_shm, off_t size);

void shm_unmap(int *ptr, off_t size);
