#include "main.h"
#include <unistd.h>

const char *reader_sem_name = "/reader-semaphore";
sem_t *reader_semaphore;
int current_reader;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("Reader received termination signal. Cleaning up...\n");

        kill(buffer->writer_pid, SIGTERM);
        kill(buffer->reader_pid[(current_reader + 1) % 2], SIGTERM);
        printf("Reader(SIGINT) ---> Writer(SIGTERM)\n");
        printf("Reader(SIGINT) ---> AnotherReader(SIGTERM)\n");

        if (sem_close(reader_semaphore) == -1) {
            perror("sem_close: Failed to close reader semaphore");
        };
        if (sem_unlink(reader_sem_name) == -1) {
            perror("sem_unlink: Failed to unlink reader semaphore");
        };

        printf("Reader: Goodbye!\n");
        exit(EXIT_SUCCESS);
    }
}

int calculate_factorial(int n) {
    int factorial = 1;
    for (int i = 1; i <= n; ++i) {
        factorial *= i;
    }
    return factorial;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    srand(time(NULL));
    init();

    if (sem_wait(admin) == -1) {
        perror("sem_wait: Failed to wait for admin semaphore");
        exit(EXIT_FAILURE);
    };

    printf("Reader %d started\n", getpid());

    if (sem_post(admin) == -1) {
        perror("sem_post: Failed to increment admin semaphore");
        exit(EXIT_FAILURE);
    }

    if ((buf_id = shm_open(shar_object, O_RDWR, 0666)) == -1) {
        perror("shm_open: Failed to access shared memory");
        exit(EXIT_FAILURE);
    } else {
        printf("Memory object is opened: name = %s, id = 0x%x\n", shar_object, buf_id);
    }

    if (ftruncate(buf_id, sizeof(shared_memory)) == -1) {
        perror("ftruncate: Failed to set memory size");
        exit(EXIT_FAILURE);
    } else {
        printf("Memory size set and = %lu\n", sizeof(shared_memory));
    }

    buffer = mmap(0, sizeof(shared_memory), PROT_WRITE | PROT_READ, MAP_SHARED, buf_id, 0);
    if (buffer == (shared_memory *) -1) {
        perror("reader: mmap: Failed to map memory");
        exit(EXIT_FAILURE);
    }

    if ((reader_semaphore = sem_open(reader_sem_name, O_CREAT, 0666, 1)) == 0) {
        perror("sem_open: Failed to create reader semaphore");
        exit(EXIT_FAILURE);
    };

    if (sem_wait(reader_semaphore) == -1) {
        perror("sem_wait: Failed to wait for reader semaphore");
        exit(EXIT_FAILURE);
    };

    if (buffer->have_reader == 2) {
        printf("Reader %d: I have lost this work :(\n", getpid());
        if (sem_post(reader_semaphore) == -1) {
            perror("sem_post: Failed to increment reader semaphore");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    current_reader = buffer->have_reader;
    ++buffer->have_reader;

    if (sem_post(reader_semaphore) == -1) {
        perror("sem_post: Failed to increment reader semaphore");
        exit(EXIT_FAILURE);
    }

    buffer->reader_pid[buffer->have_reader - 1] = getpid();
    if (buffer->have_reader == 1) buffer->next_read = 0;

    while (1) {
        sleep(rand() % 3 + 1);

        if (sem_wait(full) == -1) {
            perror("sem_wait: Failed to wait for full semaphore");
            exit(EXIT_FAILURE);
        };

        if (sem_wait(read_mutex) == -1) {
            perror("sem_wait: Failed to wait for read_mutex semaphore");
            exit(EXIT_FAILURE);
        };

        int result = buffer->store[buffer->next_read];
        buffer->store[buffer->next_read] = -1;
        int factorial = calculate_factorial(result);

        if (sem_post(empty) == -1) {
            perror("sem_post: Failed to increment empty semaphore");
            exit(EXIT_FAILURE);
        };

        pid_t pid = getpid();
        printf("Consumer %d: Reads value = %d from cell [%d], factorial = %d\n",
               pid, result, buffer->next_read, factorial);
        ++buffer->next_read;
        buffer->next_read %= BUF_SIZE;

        if (sem_post(read_mutex) == -1) {
            perror("sem_post: Failed to increment read_mutex semaphore");
            exit(EXIT_FAILURE);
        };
    }
}
