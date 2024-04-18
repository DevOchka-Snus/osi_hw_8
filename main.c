#include "common.h"
#include <semaphore.h>

const char *shared_memory_object_name = "/posix-shared-object";
int memory_object_id;
shared_memory *memory_buffer;

const char *full_semaphore_name = "/full-semaphore";
sem_t *full_semaphore;

const char *empty_semaphore_name = "/empty-semaphore";
sem_t *empty_semaphore;

const char *read_mutex_semaphore_name = "/read-mutex-semaphore";
sem_t *read_mutex_semaphore;

const char *write_mutex_semaphore_name = "/write-mutex-semaphore";
sem_t *write_mutex_semaphore;

const char *admin_semaphore_name = "/admin-semaphore";
sem_t *admin_semaphore;

void initialize(void) {
    if ((admin_semaphore = sem_open(admin_semaphore_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem_open: Can not create admin semaphore");
        exit(EXIT_FAILURE);
    }

    if ((write_mutex_semaphore = sem_open(write_mutex_semaphore_name, O_CREAT, 0666, 1)) == 0) {
        perror("sem_open: Can not create mutex semaphore");
        exit(EXIT_FAILURE);
    }

    if ((read_mutex_semaphore = sem_open(read_mutex_semaphore_name, O_CREAT, 0666, 1)) == 0) {
        perror("sem_open: Can not create mutex semaphore");
        exit(EXIT_FAILURE);
    }

    if ((empty_semaphore = sem_open(empty_semaphore_name, O_CREAT, 0666, BUF_SIZE)) == 0) {
        perror("sem_open: Can not create free semaphore");
        exit(EXIT_FAILURE);
    }

    if ((full_semaphore = sem_open(full_semaphore_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem_open: Can not create busy semaphore");
        exit(EXIT_FAILURE);
    }
}

void close_shared_semaphores(void) {
    if (sem_close(empty_semaphore) == -1) {
        perror("sem_close: Incorrect close of empty semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_close(full_semaphore) == -1) {
        perror("sem_close: Incorrect close of busy semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_close(admin_semaphore) == -1) {
        perror("sem_close: Incorrect close of admin semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_close(write_mutex_semaphore) == -1) {
        perror("sem_close: Incorrect close of mutex semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_close(read_mutex_semaphore) == -1) {
        perror("sem_close: Incorrect close of mutex semaphore");
        exit(EXIT_FAILURE);
    }
}

void remove_all(void) {
    if (sem_unlink(write_mutex_semaphore_name) == -1) {
        perror("sem_unlink: Incorrect unlink of mutex semaphore");
    }

    if (sem_unlink(read_mutex_semaphore_name) == -1) {
        perror("sem_unlink: Incorrect unlink of mutex semaphore");
    }

    if (sem_unlink(empty_semaphore_name) == -1) {
        perror("sem_unlink: Incorrect unlink of empty semaphore");
    }

    if (sem_unlink(full_semaphore_name) == -1) {
        perror("sem_unlink: Incorrect unlink of full semaphore");
    }

    if (sem_unlink(admin_semaphore_name) == -1) {
        perror("sem_unlink: Incorrect unlink of admin semaphore");
    }

    if (shm_unlink(shared_memory_object_name) == -1) {
        perror("shm_unlink");
    }
}
