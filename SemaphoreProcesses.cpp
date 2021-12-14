#include <iostream>
#include <stdio.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/types.h> 
#include <string.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 

#define SHM_KEY 0x12345
        struct shmseg {
        int counter = 1;
        int write_complete;
        int read_complete;
    };

    void nextValue(int pid, struct shmseg* shmp)
    {
        int counter;
        int numtimes;
        counter = shmp->counter;
        shmp->write_complete = 0;
        /* Pick random value between 0 and 1 for coin toss */
        int randVal = rand() % 2;
        if (randVal == 1)
        {
            if (pid == 0)
                printf("Process 1 writing to shared memory:");
            else if (pid != 0)
                printf("Process 2 writing to shared memory:");
        }
        for (numtimes = 0; numtimes < 1000; numtimes++) {
            counter += 1;
            shmp->counter = counter;
        }
        shmp->write_complete = 1;
        return;
    }

    int main(int argc, char* argv[]) {
        int shmid;
        struct shmseg* shmp; 
        char* bufptr;
        int value = 1000;
        pid_t pid; 
        shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644 | IPC_CREAT);
        if (shmid == -1) {
            perror("Shared memory");
            return 1;
        }
        shmp = shmat(shmid, NULL, 0);
        if (shmp == (void*)-1) {
            perror("Shared memory attach");
            return 1;
        }
        shmp->counter = 1;
        pid = fork();
        /* Parent Process - Writing Once */
        if (pid > 0) { nextValue(pid, shmp); }
        else if (pid == 0) {
            nextValue(pid, shmp);
            return 0;
        }
        while (shmp->read_complete != 1)
            sleep(1);
        if (shmdt(shmp) == -1) {
            perror("Error on shmp");
            return 1;
        }
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            perror("Error on shmctl");
            return 1;
        }
        return 0;
    }