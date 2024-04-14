#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define BUF_SIZE 10

typedef struct {
    pid_t writer_pid;
    pid_t reader_pid[2];
    int have_reader;
    int next_read;
    int store[BUF_SIZE];
} shared_memory;

extern const char *shar_object;
extern sem_t *admin;
extern sem_t *empty;
extern sem_t *full;
extern sem_t *read_mutex;
extern int buf_id;
extern shared_memory *buffer;

void init();

#endif /* READER_H */
