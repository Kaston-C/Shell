#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "builtin.h"
#include "executor.h"
#include "parser.h"

void handle_command(char *input) {
    Pipeline pipeline = parse_input(input);

    if (pipeline.count == 1) {
        int is_exit = execute_single_command(input, pipeline.commands[0].argv);
        if (is_exit) {
            for (int i = 0; i < pipeline.count; i++) {
                Command cmd = pipeline.commands[i];
                if (cmd.argv) {
                    for (int j = 0; cmd.argv[j] != NULL; j++) {
                        free(cmd.argv[j]);
                    }
                    free(cmd.argv);
                }
            }

            free(pipeline.commands);
            exit(0);
        }
    } else {
        execute_pipeline(input, pipeline);
    }

    for (int i = 0; i < pipeline.count; i++) {
        Command cmd = pipeline.commands[i];
        if (cmd.argv) {
            for (int j = 0; cmd.argv[j] != NULL; j++) {
                free(cmd.argv[j]);
            }
            free(cmd.argv);
        }
    }

    free(pipeline.commands);
}


int execute_single_command(char *input, char **args) {
    int redirect_idx = -1;
    char *redirect_file = NULL;
    int append_mode = 0;

    for (int i = 0; args[i] != NULL; i++) {
        if ((!strcmp(args[i], "1>") || !strcmp(args[i], ">") || !strcmp(args[i], "2>")) && args[i+1]) {
            redirect_idx = i;
            redirect_file = args[i+1];
            append_mode = 0;
            break;
        } else if ((!strcmp(args[i], "1>>") || !strcmp(args[i], ">>") || !strcmp(args[i], "2>>")) && args[i+1]) {
            redirect_idx = i;
            redirect_file = args[i+1];
            append_mode = 1;
            break;
        }
    }

    if (redirect_idx != -1) {
        int flags = O_WRONLY | O_CREAT;
        if (append_mode)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        int fd = open(redirect_file, flags, 0644);
        if (fd < 0) {
            perror("open");
            return 0;
        }

        // Fork a child to execute the command
        pid_t pid = fork();
        if (pid == 0) {
            // Redirect output
            if (!strcmp(args[redirect_idx], "2>") || !strcmp(args[redirect_idx], "2>>"))
                dup2(fd, STDERR_FILENO);
            else
                dup2(fd, STDOUT_FILENO);

            close(fd);

            // Build cleaned-up args
            char *cmd_args[redirect_idx + 1];
            for (int i = 0; i < redirect_idx; i++)
                cmd_args[i] = args[i];
            cmd_args[redirect_idx] = NULL;

            execvp(cmd_args[0], cmd_args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            close(fd);
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork");
        }

    } else {
        int executed_builtin = execute_builtin_command(input, args, stdout);
        if (!executed_builtin) {
            // External command, no redirection
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                printf("%s: command not found\n", args[0]);
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                perror("fork");
            }
        } else if (executed_builtin == 2) {
            return 1;
        }
    }

    return 0;
}

void execute_pipeline(char *input, Pipeline pipeline) {
    int count = pipeline.count;
    Command *commands = pipeline.commands;
    int pipefds[2 * (count - 1)];
    pid_t pids[count];

    // Create pipes
    for (int i = 0; i < count - 1; ++i) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    for (int i = 0; i < count; ++i) {
        int is_builtin = is_builtin_command(commands[i].argv[0]);
        int input_fd = (i == 0) ? STDIN_FILENO : pipefds[(i - 1) * 2];
        int output_fd = (i == count - 1) ? STDOUT_FILENO : pipefds[i * 2 + 1];

        if (is_builtin && i == count - 1) {
            // Built-in at end, run in parent
            dup2(input_fd, STDIN_FILENO);
            dup2(output_fd, STDOUT_FILENO);

            for (int j = 0; j < 2 * (count - 1); ++j)
                close(pipefds[j]);

            FILE *out = fdopen(STDOUT_FILENO, "w");
            if (!out) {
                perror("fdopen");
                exit(1);
            }

            execute_builtin_command(input, commands[i].argv, out);
            fflush(out);
            fclose(out);
        } else {
            // Fork for built-in (not last) or external command
            pid_t pid = fork();
            if (pid == 0) {
                dup2(input_fd, STDIN_FILENO);
                dup2(output_fd, STDOUT_FILENO);

                for (int j = 0; j < 2 * (count - 1); ++j)
                    close(pipefds[j]);

                if (is_builtin) {
                    FILE *out = fdopen(STDOUT_FILENO, "w");
                    if (!out) {
                        perror("fdopen");
                        exit(1);
                    }
                    execute_builtin_command(input, commands[i].argv, out);
                    fflush(out);
                    fclose(out);
                    exit(0);
                } else {
                    execvp(commands[i].argv[0], commands[i].argv);
                    perror("execvp");
                    exit(1);
                }
            } else if (pid > 0) {
                pids[i] = pid;
            } else {
                perror("fork");
                exit(1);
            }
        }
    }

    // Close pipe fds in parent
    for (int i = 0; i < 2 * (count - 1); ++i)
        close(pipefds[i]);

    // Wait for all forked children (built-ins and external)
    for (int i = 0; i < count; ++i) {
        // Only wait for commands that were forked
        if (!(is_builtin_command(commands[i].argv[0]) && i == count - 1))
            waitpid(pids[i], NULL, 0);
    }

    // Restore original stdin/stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
}