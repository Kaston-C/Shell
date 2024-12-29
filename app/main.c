#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 100
#define MAX_PATH 1024

void execute_external_command(char *input) {
    // Tokenize the input into command and arguments
    char *args[MAX_INPUT];
    char *token = strtok(input, " ");
    int arg_count = 0;

    while (token != NULL && arg_count < MAX_INPUT - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // Null-terminate the argument list

    if (arg_count == 0) {
        return; // No command entered
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: execute the command
        execvp(args[0], args);

        // If execvp returns, an error occurred
        fprintf(stderr, "%s: command not found\n", args[0]);
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to complete
        int status;
        waitpid(pid, &status, 0);
    }
}


int main() {
  // REPL loop
  while (1) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    // Wait for user input
    char input[MAX_INPUT];
    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        // Handle end-of-file (Ctrl+D)
        printf("\n");
        break;
    }
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
            char *pathdup = strndup(path, strlen(path));

            if (path == NULL) {
                printf("%s: not found\n", command);
                continue;
            }

            char *dir = strtok(pathdup, ":"); // Split PATH into directories
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

            free(pathdup); // Free the duplicated path string
        }
    // Handle unrecognized commands
    } else {
        execute_external_command(input);
    }
  }
  return 0;
}