#ifndef WRITER_H
#define WRITER_H

#include "main.h"
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

extern const char *writer_semaphore_name;
extern sem_t *writer_semaphore;
extern const char *first_writer_semaphore_name;
extern sem_t *first_writer_semaphore;

void handle_signal(int sig);

#endif /* WRITER_H */
