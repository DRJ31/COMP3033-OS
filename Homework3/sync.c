#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N 2 // Total number of threads
#define M 100 // Number of loop iterations per thread

int sum = 0;
int flag[N] = {0}; // Initialize array for flag

// The function executed by each thread.
void *runner(void *param) {
    int i = *(int *)param; // This thread’s ID number.
    int m, j = 0;
    
    for(m = 0; m < M; m++) {
        // Pi wants to enter the waiting room:
        flag[i] = 1;
        while (flag[j] >= 3); // Atomic test
        // Pi goes through the entrance door
        flag[i] = 3;
        // CHeck whether Pj wants to enter the waiting room
        if (flag[j] == 1) {
            // Then Pi starts waiting inside the waiting room
            flag[i] = 2;
            // Until Pj comes inside the waiting room too and the entrance
            // door closes
            while (flag[j] != 4);
        }
        if (j < i) {
            while (flag[j] >= 2); // Atomic test
        }
        
        // The Critical Section starts right below
        int s = sum;
        // Even threads increase s, odd threads decrease s.
        if(i % 2 == 0) {
            s++;
        }
        else {
            s--;
        }
        
        // Sleep a small random amount of time. Do not remove this code.
        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 100000000ULL * rand() / RAND_MAX;
        nanosleep(&delay, NULL);

        sum = s;
        // The Critical Section starts right below
        
        if (j > i) {
            while (flag[j] == 3 || flag[j] == 2);
        }
        
        flag[i] = 0;
        
        printf("%c", 'A' + i); // Print this thread’s ID number as a letter.
        fflush(stdout);
    }
    return 0; // Thread dies.
}
