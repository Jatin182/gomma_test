#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, SHM_SIZE, 0666|IPC_CREAT);
    int *shared_memory = (int*) shmat(shmid, (void*)0, 0);

    // Initialize shared memory
    for (int i = 0; i < 5; i++) {
        shared_memory[i] = i + 1;
    }

    // Create child processes
    pid_t pids[5];
    for (int i = 0; i < 5; i++) {
        if ((pids[i] = fork()) == 0) {
            shared_memory[i] *= shared_memory[i];
            shmdt(shared_memory);
            exit(0);
        }
    }

    // Wait for child processes to complete
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    // Print the modified shared memory
    printf("Modified shared memory: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", shared_memory[i]);
    }
    printf("\n");

    // Detach from shared memory
    shmdt(shared_memory);

    // Destroy the shared memory
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
