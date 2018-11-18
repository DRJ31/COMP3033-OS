#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N       26 // Total number of threads
#define M       10 // Number of loop iterations per thread
#define TRUE    1
#define FALSE   0

int sum = 0;
int flag[N] = {0}; // Initialize array for flag

// Check if value is in the array
// length: Length of range array
int numInArr(int self, int value, const int range[], int length) {
    for (int i = 0; i < length; i++) {
        if (value == range[i] && i != self)
            return TRUE;
    }
    return FALSE;
}

// Wait for array until it meets condition
// length: Length of range array
// start: The first point of flag's range
// end: The last point of flag's range
// init: TRUE -> all the elements in flag, FALSE -> any element in flag
void await(int self, int flag[], int start, int end, const int range[], int length, int init) {
    int opened = init; // Check if the door has opened
    if (start > end)
        return;
    while (opened == init) {
        for (int i = start; i <= end; i++) {
            if (numInArr(self, flag[i], range, length)) {
                opened = opened ? FALSE : TRUE;
                break;
            }
        }
    }
}

// The function executed by each thread.
void *runner(void *param) {
    int i = *(int *)param; // This thread’s ID number.
    int m;
    //    int j = i == 1 ? 0 : 1;
    int openDoor[] = {0, 1, 2}; // Range for waiting open door
    int closeDoor[] = {4}; // Range for waiting door close
    int finishExit[] = {0, 1}; // Range for waiting other lower ID to finish exit
    int waitClose[] = {0, 1, 4}; // Range for ensure every process know door is to be close
    
    for(m = 0; m < M; m++) {
        // Pi wants to enter the waiting room:
        flag[i] = 1;
        await(i, flag, 0, N - 1, openDoor, 3, TRUE); // Atomic test
        // Pi goes through the entrance door
        flag[i] = 3;
        // Check whether other process wants to enter the waiting room
        if (numInArr(i, 1, flag, N)) {
            // Then Pi starts waiting inside the waiting room
            flag[i] = 2;
            // Until Pj comes inside the waiting room too and the entrance
            // door closes
            await(i, flag, 0, N - 1, closeDoor, 1, FALSE); // Wait for a process to enter and close door
        }
        // In all cases, flag[i] becomes 4 (that’s also when the entrance door
        // automatically closes).
        flag[i] = 4;
        await(i, flag, 0, i - 1, finishExit, 2, TRUE); // Wait for all lower ID to finish exit protocol
        
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
        await(i, flag, i + 1, N - 1, waitClose, 3, TRUE); // Ensure every process know the door is to be close
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

