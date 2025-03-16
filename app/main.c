#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT 100
#define MAX_PATH 1024

char** process_input(char *input);
void write_out(char *input, char **args);
void write_to_file(char *input, char *file_name, char **args);
void execute_external_command(char **args);
void execute_external_command_write_to_file(char **args, char *file_name);

int main() {
    // REPL loop
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");

        // Wait for user input
        char input[MAX_INPUT];
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            // Handle end-of-file
            printf("\n");
            break;
        }
        // Set newline to null terminator
        input[strlen(input) - 1] = '\0';

        char **args = process_input(input);

        int to_file = 0;
        for (int i = 0; args[i] != NULL; i++) {
            if ((!strcmp(args[i], "1>") || !strcmp(args[i], ">")) && args[i + 1] != NULL) {
                write_to_file(input, args[i + 1], args);
                to_file = 1;
                break;
            }
        }

        if (!to_file) {
            write_out(input, args);
        }

        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    }
    return 0;
}

char** process_input(char *input) {
    int len = strlen(input);
    char temp[MAX_INPUT];
    char token[MAX_INPUT];
    char **args = malloc(MAX_INPUT * sizeof(char*));
    int arg_count = 0;
    int j = 0;
    int k = 0;

    for (int i = 0; i < len; i++) {
        if (input[i] == '\'') {
            i++;

            while (input[i] != '\'' && input[i] != '\0') {
                temp[j++] = input[i];
                token[k++] = input[i++];
            }

            token[k] = '\0';
            args[arg_count] = strdup(token);
            arg_count++;
            k = 0;
        } else if (input[i] == '"') {
            i++;

            while (input[i] != '"' && input[i] != '\0') {
                if (input[i] == '\\') {
                    if (input[i + 1] == '\\' || input[i + 1] == '$' || input[i + 1] == '"' || input[i + 1] == '\n') {
                        i++;
                    }
                }
                temp[j++] = input[i];
                token[k++] = input[i++];
            }

            token[k] = '\0';
            args[arg_count] = strdup(token);
            arg_count++;
            k = 0;
        } else if (isspace(input[i])) {
            temp[j++] = input[i];

            while (isspace(input[i])) {
                i++;
            }
            i--;
        } else {
            while (input[i] != '\0' && !isspace(input[i]) && input[i] != '\'' && input[i] != '"') {
                if (input[i] == '\\') {
                    i++;
                }
                temp[j++] = input[i];
                token[k++] = input[i++];
            }
            token[k] = '\0';
            args[arg_count++] = strdup(token);
            k = 0;
            i--;
        }
    }

    args[arg_count] = NULL;
    temp[j] = '\0';

    strcpy(input, temp);

    return args;
}

void write_out(char *input, char **args) {
    // Handle exit command
        if (strncmp(input, "exit 0", 6) == 0) {
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }
            free(args);
            exit(0);
        // Handle echo command
        } else if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);
        // Handle pwd command
        } else if (strncmp(input, "pwd", 3) == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd failed");
            }
        // Handle cd command
        } else if (strncmp(input, "cd", 2) == 0) {
            if (strncmp(input + 2, "", 1) == 0 || strncmp(input + 3, "~", 1) == 0) {
                chdir(getenv("HOME"));
            } else if (chdir(input + 3) != 0) {
                printf("cd:%s: No such file or directory\n", input + 2);
            } else {
                return;
            }
        // Handle type command
        } else if (strncmp(input, "type ", 5) == 0) {
            if (strncmp(input + 5, "exit", 4) == 0) {
                printf("exit is a shell builtin\n");
            } else if (strncmp(input + 5, "echo", 4) == 0) {
                printf("echo is a shell builtin\n");
            } else if (strncmp(input + 5, "type", 4) == 0) {
                printf("type is a shell builtin\n");
            } else if (strncmp(input + 5, "pwd", 3) == 0) {
                printf("pwd is a shell builtin\n");
            } else {
                char *command = input + 5;
                char *path = getenv("PATH"); // Get the PATH environment variable
                char *pathdup = strndup(path, strlen(path));

                if (path == NULL) {
                    printf("%s: not found\n", command);
                    return;
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
        // Execute unrecognized commands
        } else {
            execute_external_command(args);
        }
}

void write_to_file(char *input, char *file_name, char **args) {
    if (strncmp(input, "exit 0", 6) == 0) {
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }
            free(args);
            exit(0);
        // Handle echo command
        } else if (strncmp(input, "echo ", 5) == 0) {
            FILE *file = fopen(file_name, "w");

            char *redirect_pos = strstr(input + 5, ">");
            *redirect_pos = '\0';
            if (*(redirect_pos - 1) == '1') {
                *(redirect_pos - 1) = '\0';
            }

            if (file == NULL) {
                perror("Error opening file");
            } else {
                fprintf(file, "%s\n", input + 5);
            }

            fclose(file);
        } else if (strncmp(input, "pwd", 3) == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                FILE *file = fopen(file_name, "w");

                if (file == NULL) {
                    perror("Error opening file");
                } else {
                    fprintf(file, "%s\n", cwd);
                }

                fclose(file);
            } else {
                perror("getcwd failed");
            }
        // Handle cd command
        } else if (strncmp(input, "cd", 2) == 0) {
            if (strncmp(input + 2, "", 1) == 0 || strncmp(input + 3, "~", 1) == 0) {
                chdir(getenv("HOME"));
            } else if (chdir(input + 3) != 0) {
                printf("cd:%s: No such file or directory\n", input + 2);
            } else {
                return;
            }
        // Handle type command
        } else if (strncmp(input, "type ", 5) == 0) {
            FILE *file = fopen(file_name, "w");

            if (file == NULL) {
                perror("Error opening file");
            } else {
                if (strncmp(input + 5, "exit", 4) == 0) {
                    fprintf(file, "exit is a shell builtin\n");
                } else if (strncmp(input + 5, "echo", 4) == 0) {
                    fprintf(file, "echo is a shell builtin\n");
                } else if (strncmp(input + 5, "type", 4) == 0) {
                    fprintf(file, "type is a shell builtin\n");
                } else if (strncmp(input + 5, "pwd", 3) == 0) {
                    fprintf(file, "pwd is a shell builtin\n");
                } else {
                    char *command = input + 5;
                    char *path = getenv("PATH"); // Get the PATH environment variable
                    char *pathdup = strndup(path, strlen(path));

                    if (path == NULL) {
                        printf("%s: not found\n", command);
                        return;
                    }

                    char *dir = strtok(pathdup, ":"); // Split PATH into directories
                    int found = 0;

                    while (dir != NULL) {
                        // Construct the full path to the command
                        char full_path[MAX_PATH];
                        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

                        // Check if the file exists and is executable
                        if (access(full_path, X_OK) == 0) {
                            fprintf(file, "%s is %s\n", command, full_path);
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
            }
        // Execute unrecognized commands
        } else {
            execute_external_command_write_to_file(args, file_name);
        }
    }

void execute_external_command(char **args) {
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

void execute_external_command_write_to_file(char **args, char *file_name) {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);

        int i;
        for (i = 0; args[i] != NULL; i++) {
            if (!strcmp(args[i], "1>") || !strcmp(args[i], ">")) {
                break;
            }
        }

        char *cmd_args[i + 1];
        for (int j = 0; j < i; j++) {
            cmd_args[j] = args[j];
        }
        cmd_args[i] = NULL;

        // Child process: execute the command
        execvp(args[0], args);

        // If execvp returns, an error occurred
        fprintf(stderr, "Error executing command '%s': ", cmd_args[0]);
        perror("");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to complete
        int status;
        waitpid(pid, &status, 0);
    }
}