#include <stdio.h>
#include <stdlib.h>

#define FINISHED    0
#define UNFINISHED  1
#define LOCKED      1
#define UNLOCKED    0

typedef struct {
    int id; // ID of a process
    int burstTime; // Burst time of a process
    int period; // Period of a process
    int deadline; // Time of next deadline
    int remainTime; // Remained execution time before deadline
    int status; // Whether the program has finished before deadline
    int lock; // Lock the change of status when necessary
    int waitingTime; // Total waiting time of a process
    int lastTime; // The last time the process execute
} Process;


// Greatest Common Divisor
int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}


// Least Common Mutiple
int lcm(int a[], int num) {
    int result = 1;
    for (int i = 0; i < num; i++) {
        result *= a[i] / gcd(result, a[i]);
    }
    return result;
}


// Get the process which has earliest deadline
Process *getEarliestDeadline(Process *processes, int processNum) {
    int smallest = 0;
    for (int i = 1; i < processNum; i++) {
        if (processes[i].deadline < processes[smallest].deadline)
            smallest = i;
    }
    return &processes[smallest];
}


// Get the next process to run
// earlier: Process which has finished before deadline
Process *getCurrentProcess(Process *processes, int processNum, Process *earlier) {
    Process *current = NULL;
    // Ensure current is not the process in parameter
    for (int i = 0; i < processNum; i++) {
        if (processes[i].status == UNFINISHED) {
            current = &processes[i];
            break;
        }
    }
    // If all the processes have finished, return a process whose id is -1
    if (current == NULL)
        return current;
    // Find the process which has the earliest deadline
    for (int i = 1; i < processNum; i++) {
        if (processes[i].deadline < current->deadline && processes[i].status == UNFINISHED)
            current = &processes[i];
    }
    return current;
}


// Update the deadline of a process
void updateDeadline(Process *process) {
    process->deadline += process->period;
}


// Update the status of a process
void updateStatus(Process *process, int status) {
    if (!process->lock)
        process->status = status;
    if (status == FINISHED)
        process->remainTime = process->burstTime;
}


// Get total waiting time of all the processses
int getWaitingTime(Process *processes, int processNum) {
    int waitingTime = 0;
    for (int i = 0; i < processNum; i++) {
        waitingTime += processes[i].waitingTime;
    }
    return waitingTime;
}


// Get waiting time for current process
int getCurrentWaitingTime(Process *process, int currentTime) {
    if (process->lock)
        return currentTime - process->lastTime;
    return currentTime - process->deadline + process->period;
}


// Initialize information of processes and return Max Time
int initialize(Process *processes, int processNum) {
    int period[processNum]; // Temporary array to calculate Max Time
    for (int i = 0; i < processNum; i++) {
        processes[i].id = i + 1;
        printf("Enter burst time for process %d: ", processes[i].id);
        scanf("%d", &processes[i].burstTime);
        printf("Enter period for process %d: ", processes[i].id);
        scanf("%d", &processes[i].period);
        processes[i].deadline = processes[i].period;
        period[i] = processes[i].period;
        processes[i].remainTime = processes[i].burstTime;
        processes[i].status = UNFINISHED;
        processes[i].lock = UNLOCKED;
        processes[i].waitingTime = 0;
        processes[i].lastTime = 0;
    }
    return lcm(period, processNum);
}


int main(void) {
    // Variable declaration
    Process *processes; // Array containing information for all the process
    Process *current = NULL; // Current running process
    Process *earliest; // Information for earliest deadline process
    Process *former; // Record last process
    int maxTime; // End time for all the process
    int processNum; // Number of processes
    int currentTime = 0; // Record running time of process
    int burst = 0; // Counter of CPU burst time
    
    // Initialization
    printf("Enter number of process to schedule: ");
    scanf("%d", &processNum);
    processes = (Process *)malloc(sizeof(Process) * processNum); // Initialize processes
    maxTime = initialize(processes, processNum);
    
    // Main process
    while (currentTime < maxTime) {
        earliest = getEarliestDeadline(processes, processNum);
        former = current;
        current = earliest;
        // Choose correct process to run
        if (current->status == FINISHED)
            current = getCurrentProcess(processes, processNum, current);
        // Situation when all the process has done
        if (current == NULL) {
            currentTime = earliest->deadline;
            updateDeadline(earliest);
            updateStatus(earliest, UNFINISHED);
            continue;
        }
        // Decide if we need to switch to new process
        if (former != NULL) {
            if (former->id != current->id) {
                if (former->remainTime < former->burstTime) {
                    printf("%d: process %d preempted!\n", currentTime, former->id);
                    burst--;
                }
                printf("%d: process %d starts\n", currentTime, current->id);
                current->waitingTime += getCurrentWaitingTime(current, currentTime);
                burst++;
            }
        }
        else {
            printf("%d: process %d starts\n", currentTime, current->id);
            current->waitingTime += getCurrentWaitingTime(current, currentTime);
            burst++;
        }
        // Deal with different situations
        if (currentTime + current->remainTime <= earliest->deadline) {
            currentTime += current->remainTime;
            updateStatus(current, FINISHED);
            current->lock = UNLOCKED;
            // Check if the process has lock before
            if (current->status == FINISHED)
                printf("%d: process %d ends\n", currentTime, current->id);
            // Check if we need to update deadline
            if (currentTime == earliest->deadline) {
                updateDeadline(earliest);
                updateStatus(earliest, UNFINISHED);
            }
        }
        else {
            current->remainTime = currentTime + current->remainTime - earliest->deadline;
            currentTime = earliest->deadline;
            if (current->id == earliest->id) {
                printf("%d: process %d missed deadline (%d ms left)\n", currentTime, current->id, current->remainTime);
                current->lock = LOCKED;
                current->lastTime = currentTime;
            }
            updateDeadline(earliest);
            updateStatus(earliest, UNFINISHED);
        }
    }
    
    // Printing results
    printf("%d: MaxTime reached\n", maxTime);
    printf("Sum of all waiting time: %d\n", getWaitingTime(processes, processNum));
    printf("Number of CPU bursts: %d\n", burst);
    printf("Average waiting time: %.6f\n", getWaitingTime(processes, processNum) * 1.0 / burst);
    return 0;
}