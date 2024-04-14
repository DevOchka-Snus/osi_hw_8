#include "main.h"
#include <semaphore.h>
#include <signal.h>

const char *writer_semaphore_name = "/writer-semaphore";
sem_t *writer_semaphore;
const char *first_writer_semaphore_name = "/first-writer-semaphore";
sem_t *first_writer_semaphore;

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("Writer received termination signal. Cleaning up...\n");

        kill(buffer->reader_pid[0], SIGTERM);
        kill(buffer->reader_pid[1], SIGTERM);
        printf("Writer(SIGINT) ---> Readers(SIGTERM)\n");

        if (sem_close(writer_semaphore) == -1) {
            perror("sem_close: Failed to close writer semaphore");
        };
        if (sem_close(first_writer_semaphore) == -1) {
            perror("sem_close: Failed to close first_writer semaphore");
        };
        close_common_semaphores();

        if (sem_unlink(writer_semaphore_name) == -1) {
            perror("sem_unlink: Failed to unlink writer semaphore");
        };
        if (sem_unlink(first_writer_semaphore_name) == -1) {
            perror("sem_unlink: Failed to unlink first_writer semaphore");
        };
        unlink_all();
        printf("Writer: Goodbye!\n");
        exit(EXIT_SUCCESS);
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    srand(time(0));
    init();

    if ((buf_id = shm_open(shar_object, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    } else {
        printf("Object is open: name = %s, id = 0x%x\n", shar_object, buf_id);
    }

    if (ftruncate(buf_id, sizeof(shared_memory)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    } else {
        printf("Memory size set and = %lu\n", sizeof(shared_memory));
    }

    buffer = mmap(0, sizeof(shared_memory), PROT_WRITE | PROT_READ, MAP_SHARED, buf_id, 0);
    if (buffer == (shared_memory *) -1) {
        perror("writer: mmap");
        exit(EXIT_FAILURE);
    }
    printf("mmap checkout\n");

    if ((writer_semaphore = sem_open(writer_semaphore_name, O_CREAT, 0666, 1)) == 0) {
        perror("sem_open: Failed to create writer semaphore");
        exit(EXIT_FAILURE);
    };

    if ((first_writer_semaphore = sem_open(first_writer_semaphore_name, O_CREAT, 0666, 1)) == 0) {
        perror("sem_open: Failed to create first_writer semaphore");
        exit(EXIT_FAILURE);
    };

    if (sem_wait(writer_semaphore) == -1) {
        perror("sem_wait: Failed to wait for writer semaphore");
        exit(EXIT_FAILURE);
    };

    int writer_number = 0;
    sem_getvalue(first_writer_semaphore, &writer_number);
    printf("Checking: writer_number = %d\n", writer_number);

    if (writer_number == 0) {
        printf("Writer %d: I have lost this work :(\n", getpid());
        if (sem_post(writer_semaphore) == -1) {
            perror("sem_post: Failed to increment writer semaphore");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    if (sem_wait(first_writer_semaphore) == -1) {
        perror("sem_wait: Failed to wait for first_writer semaphore");
        exit(EXIT_FAILURE);
    };

    if (sem_post(writer_semaphore) == -1) {
        perror("sem_post: Failed to increment writer semaphore");
        exit(EXIT_FAILURE);
    }

    buffer->writer_pid = getpid();
    printf("Writer %d: I am first for this work! :)\n", getpid());

    for (int i = 0; i < BUF_SIZE; ++i) {
        buffer->store[i] = -1;
    }
    buffer->have_reader = 0;

    int is_writers = 0;
    sem_getvalue(admin, &is_writers);
    if (is_writers == 0) {
        if (sem_post(admin) == -1) {
            perror("sem_post: Can not increment admin semaphore");
            exit(EXIT_FAILURE);
        }
    }

    buffer->next_write = 0;

    while (1) {
        if (sem_wait(empty) == -1) {
            perror("sem_wait: Failed to wait for empty semaphore");
            exit(EXIT_FAILURE);
        }

        if (sem_wait(write_mutex) == -1) {
            perror("sem_wait: Failed to wait for write_mutex semaphore");
            exit(EXIT_FAILURE);
        }

        buffer->store[buffer->next_write] = rand() % 11;

        if (sem_post(full) == -1) {
            perror("sem_post: Failed to increment full semaphore");
            exit(EXIT_FAILURE);
        };

        pid_t pid = getpid();
        printf("Producer %d writes value = %d to cell [%d]\n",
               pid, buffer->store[buffer->next_write], buffer->next_write);
        ++buffer->next_write;
        buffer->next_write %= BUF_SIZE;

        if (sem_post(write_mutex) == -1) {
            perror("sem_post: Failed to increment write_mutex semaphore");
            exit(EXIT_FAILURE);
        }
        sleep(rand() % 3 + 1);
    }
    return 0;
}
