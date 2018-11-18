#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N       2 // Total number of threads
#define M       100 // Number of loop iterations per thread
#define TRUE    1
#define FALSE   0

int sum = 0;
int flag[N] = {0}; // Initialize array for flag

// Check if value is in the array
// length: Length of range array
int numInArr(int value, const int range[], int length) {
    for (int i = 0; i < length; i++) {
        if (value == range[i])
            return TRUE;
    }
    return FALSE;
}

// Wait for array until it meets condition
// length: Length of range array
void await(int flag[], const int range[], int length) {
    int open; // Check if the door has opened
    while (1) {
        open = TRUE;
        for (int i = 0; i < N; i++) {
            if (numInArr(flag[i], range, length))
                open = FALSE;
        }
        if (open)
            break;
    }
}

// The function executed by each thread.
void *runner(void *param) {
    int i = *(int *)param; // This thread’s ID number.
    int m;
    int j = i == 1 ? 0 : 1;
    int openDoor[] = {0, 1, 2}; // Range for waiting open door
    int closeDoor[] = {4}; // Range for waiting door close
    int finishExit[] = {0, 1}; // Range for waiting other lower ID to finish exit
    int waitClose[] = {0, 1, 4}; // Range for ensure every process know door is to be close
    
    for(m = 0; m < M; m++) {
        // Pi wants to enter the waiting room:
        flag[i] = 1;
//        await(flag, openDoor, 3); // Atomic test
        while (flag[j] >= 3);
        // Pi goes through the entrance door
        flag[i] = 3;
        // Check whether other process wants to enter the waiting room
//        if (numInArr(1, flag, N)) {
        if (flag[j] == 1) {
            // Then Pi starts waiting inside the waiting room
            flag[i] = 2;
            // Until Pj comes inside the waiting room too and the entrance
            // door closes
//            await(flag, closeDoor, 1); // Wait for a process to enter and close door
            while (flag[j] != 4);
        }
        // In all cases, flag[i] becomes 4 (that’s also when the entrance door
        // automatically closes).
        flag[i] = 4;
//        await(flag, finishExit, 2); // Wait for all lower ID to finish exit protocol
        if (j < i) {
            while (flag[j] >= 2);
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
        // The Critical Section ends right above

        // Exit protocol
//        await(flag, waitClose, 3); // Ensure every process know the door is to be close
        if (j > i) {
            while (flag[j] == 3 || flag[j] == 2);
        }
        flag[i] = 0; // Leave, reopen door if nobody still in waiting room

        // The Remainder Section starts here
        printf("%c", 'A' + i); // Print this thread’s ID number as a letter.
        fflush(stdout);
    }
    return 0; // Thread dies.
}

int main(void) {
    pthread_t tid[N]; // Thread ID numbers.
    int param[N]; // One parameter for each thread.
    int i;

    // Create N threads. Each thread executes the runner function with
    // i as argument.
    for(i = 0; i < N; i++) {
        param[i] = i;
        pthread_create(&tid[i], NULL, runner, &param[i]);
    }

    // Wait for N threads to finish.
    for(i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("\nResult is %d\n", sum);
    return 0;
}

