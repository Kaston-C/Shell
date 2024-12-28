#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUT 100
#define MAX_PATH 1024

int main() {
  // REPL loop
  while (1) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    // Wait for user input
    char input[MAX_INPUT];
    fgets(input, MAX_INPUT, stdin);
    // Set newline to null terminator
    input[strlen(input) - 1] = '\0';

    // Handle exit command
    if (strncmp(input, "exit 0", 6) == 0) {
        exit(0);
    // Handle echo command
    } else if (strncmp(input, "echo ", 5) == 0) {
        printf("%s\n", input + 5);
    // Handle the type command
    } else if (strncmp(input, "type ", 5) == 0) {
        if (strncmp(input + 5, "exit", 4) == 0) {
            printf("exit is a shell builtin\n");
        } else if (strncmp(input + 5, "echo", 4) == 0) {
            printf("echo is a shell builtin\n");
        } else if (strncmp(input + 5, "type", 4) == 0) {
            printf("type is a shell builtin\n");
        } else {
            char *command = input + 5;
            char *path = getenv("PATH"); // Get the PATH environment variable

            if (path == NULL) {
                printf("%s: not found\n", command);
                continue;
            }

            char *dir = strtok(path, ":"); // Split PATH into directories
            int found = 0;

            while (dir != NULL) {
                // Construct the full path to the command
                char full_path[MAX_PATH];
                snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

                // Check if the file exists and is executable
                if (access(full_path, X_OK) == 0) {
                    printf("%s is %s\n", command, full_path);
                    found = 1;
                    break; // Exit the loop after the first match
                }

                dir = strtok(NULL, ":"); // Move to the next directory in PATH
            }

            if (!found) {
                printf("%s: not found\n", command);
            }
        }
    // Handle unrecognized commands
    } else {
        printf("%s: command not found\n", input);
    }
  }

  return 0;
}
