#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "builtin.h"

int is_builtin_command(const char* cmd) {
    return strcmp(cmd, "cd") == 0 ||
           strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "type") == 0 ||
           strcmp(cmd, "echo") == 0;
}

int execute_builtin_command(char *input, char **args, FILE *output_stream) {
    if (strncmp(args[0], "exit", 4) == 0) {
        return 2;
    } else if (strcmp(args[0], "echo") == 0) {
        fprintf(output_stream, "%s\n", input + 5);
    } else if (strncmp(args[0], "pwd", 3) == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            fprintf(output_stream, "%s\n", cwd);
        } else {
            fprintf(stderr, "getcwd failed\n");
        }
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || strcmp(args[1], "~") == 0) {
            chdir(getenv("HOME"));
        } else if (chdir(args[1]) != 0) {
            printf("cd: %s: No such file or directory\n", args[1]);
        }
    } else if (strncmp(args[0], "type", 5) == 0) {
        if (strncmp(args[1], "exit", 4) == 0) {
            fprintf(output_stream, "exit is a shell builtin\n");
        } else if (strncmp(args[1], "echo", 4) == 0) {
            fprintf(output_stream, "echo is a shell builtin\n");
        } else if (strncmp(args[1], "type", 4) == 0) {
            fprintf(output_stream, "type is a shell builtin\n");
        } else if (strncmp(args[1], "pwd", 3) == 0) {
            fprintf(output_stream, "pwd is a shell builtin\n");
        } else if (strncmp(args[1], "cd", 2) == 0) {
            fprintf(output_stream, "cd is a shell builtin\n");
        } else {
            char *command = args[1];
            char *path = getenv("PATH"); // Get the PATH environment variable
            char *pathdup = strndup(path, strlen(path));

            if (path == NULL) {
                fprintf(output_stream, "%s: not found\n", command);
                return 1;
            }

            char *dir = strtok(pathdup, ":"); // Split PATH into directories
            int found = 0;

            while (dir != NULL) {
                // Construct the full path to the command
                char full_path[MAX_PATH];
                snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

                // Check if the file exists and is executable
                if (access(full_path, X_OK) == 0) {
                    fprintf(output_stream, "%s is %s\n", command, full_path);
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
    } else {
        return 0; // Not built-in
    }
    return 1; // Built-in executed
}