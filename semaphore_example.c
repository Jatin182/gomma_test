#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

#define NUM_PROCESSES 5

int main() {
    // Create and initialize semaphore
    sem_t *sem = sem_open("/semaphore_example", O_CREAT, 0644, 1);

    // Shared memory for counter
    int *shared_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_counter = 0;

    // Create child processes
    pid_t pids[NUM_PROCESSES];
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if ((pids[i] = fork()) == 0) {
            // Wait for random time to simulate work
            usleep(rand() % 1000000);

            // Increment shared counter with semaphore protection
            sem_wait(sem);
            (*shared_counter)++;
            printf("Process %d incremented counter to %d\n", getpid(), *shared_counter);
            sem_post(sem);

            munmap(shared_counter, sizeof(int));
            sem_close(sem);
            exit(0);
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // Print the final value of the shared counter
    printf("Final counter value: %d\n", *shared_counter);

    // Clean up
    munmap(shared_counter, sizeof(int));
    sem_close(sem);
    sem_unlink("/semaphore_example");

    return 0;
}
