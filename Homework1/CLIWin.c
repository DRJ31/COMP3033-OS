#include <string.h>
#include <Windows.h>
#define SIZE 1024

int main(void) {
	char prompt[] = "Type a command: ";
	char buf[SIZE];
	DWORD writeLength;
	DWORD readLength;
	STARTUPINFO si; // For startup information
	PROCESS_INFORMATION pi; // For process information
	DWORD status;
	HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);

	while (1) {
		// Ask the user to type a command:
		WriteConsole(writeHandle, prompt, strlen(prompt), &writeLength, NULL);
		// Read from the standard input the command typed by the user (note 
		// that fgets also puts into the array buf the ’\n’ character typed 
		// by the user when the user presses the Enter key on the keyboard):
		ReadConsole(readHandle, buf, SIZE, &readLength, NULL);
		// Replace the Enter key typed by the user with ’\0’:
		for (int i = 0; i < SIZE; i++) {
			if (buf[i] == '\n' || buf[i] == '\r') {
				buf[i] = '\0';
				break;
			}
		}
		// Execute the command typed by the user (only prints it for now):
		if (strcmp("exit", buf) == 0) // Exit when user type "exit"
			return 0;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		if (!CreateProcess(NULL, buf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			char errorMsg[] = "CreateProcess failed!\n";
			WriteConsole(writeHandle, errorMsg, strlen(errorMsg), &writeLength, NULL);
			continue;
		}
		status = WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}