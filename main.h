#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#define BUF_SIZE 10

typedef struct {
    pid_t reader_pid[2];
    pid_t writer_pid;
    int store[BUF_SIZE];
    int have_reader;
    int next_read;
    int next_write;
} shared_memory;

extern const char *shared_memory_object_name;
extern int memory_object_id;
extern shared_memory *memory_buffer;

extern const char *full_semaphore_name;
extern sem_t *full_semaphore;

extern const char *empty_semaphore_name;
extern sem_t *empty_semaphore;

extern const char *read_mutex_semaphore_name;
extern sem_t *read_mutex_semaphore;

extern const char *write_mutex_semaphore_name;
extern sem_t *write_mutex_semaphore;

extern const char *admin_semaphore_name;
extern sem_t *admin_semaphore;

void initialize(void);
void close_shared_semaphores(void);
void remove_all(void);

#endif /* COMMON_H */
