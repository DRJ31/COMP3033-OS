#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define SIZE 1024

int main(void) {
    char prompt[] = "Type a command: "; 
    char buf[SIZE];
    while (1) {
        // Ask the user to type a command:
        write(1, prompt, strlen(prompt));
        // Read from the standard input the command typed by the user (note 
        // that fgets also puts into the array buf the ’\n’ character typed 
        // by the user when the user presses the Enter key on the keyboard): 
        read(0, buf, SIZE);
        // Replace the Enter key typed by the user with ’\0’:
        for(int i = 0; i < SIZE; i++) {
            if(buf[i] == '\n'|| buf[i] == '\r') {
                buf[i] = '\0';
                break;
            }   
        } 
        // Execute the command typed by the user (only prints it for now):
        if (strcmp("exit", buf) == 0) //Exit when user type "exit"
            return 0;
        pid_t pid; 
        int status; //The status of child process
        pid = fork();
        if (pid < 0) {
            write(1, "Fork Failed!\n", strlen("Fork Failed!\n")); //Stop when failed to create child process
            return 1;
        }
        else if (pid == 0) {
            execlp(buf, buf, NULL);
            //Stop when child proess execution failed
            write(1, "Failed to execute the program!\n", strlen("Failed to execute the program!\n")); 
            return 1;
        }
        else {
            wait(&status); //Wait for child process to stop
        }
    }
}