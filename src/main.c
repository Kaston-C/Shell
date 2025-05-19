#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include "executor.h"

char *command_generator(const char *text, int state);
char **completion(const char *text, int start, int end);

int main() {
    rl_attempted_completion_function = completion;

    while (1) {
        setbuf(stdout, NULL);
        char prompt[1024];

        snprintf(prompt, sizeof(prompt), "$ ");

        char *input = readline(prompt);
        if (!input) {
            printf("\n");
            break;
        }

        if (*input)
            add_history(input);

        handle_command(input);

        free(input);
    }

    return 0;
}

const char *builtins[] = {"exit", "echo", "pwd", "cd", "type", NULL};

char *command_generator(const char *text, int state) {
    static int match_index = 0;
    static int total_matches = 0;
    static char **matches = NULL;

    if (state == 0) {
        match_index = 0;
        total_matches = 0;

        if (matches) {
            for (int i = 0; i < total_matches; i++)
                free(matches[i]);
            free(matches);
            matches = NULL;
        }

        matches = malloc(1024 * sizeof(char*));

        // Add built-ins
        for (int i = 0; builtins[i]; i++) {
            if (strncmp(builtins[i], text, strlen(text)) == 0) {
                matches[total_matches++] = strdup(builtins[i]);
            }
        }

        // Add commands from PATH
        const char *path_env = getenv("PATH");
        if (path_env) {
            char *paths = strdup(path_env);
            char *dir = strtok(paths, ":");
            while (dir) {
                DIR *dp = opendir(dir);
                if (dp) {
                    struct dirent *entry;
                    while ((entry = readdir(dp)) != NULL) {
                        if (strncmp(entry->d_name, text, strlen(text)) == 0) {
                            // Skip duplicates
                            int duplicate = 0;
                            for (int i = 0; i < total_matches; i++) {
                                if (strcmp(matches[i], entry->d_name) == 0) {
                                    duplicate = 1;
                                    break;
                                }
                            }
                            if (!duplicate)
                                matches[total_matches++] = strdup(entry->d_name);
                        }
                    }
                    closedir(dp);
                }
                dir = strtok(NULL, ":");
            }
            free(paths);
        }
    }

    if (match_index < total_matches) {
        return strdup(matches[match_index++]);
    }

    return NULL;
}

char *path_generator(const char *text, int state) {
    // This can utilize file and directory completion from readline
    return rl_filename_completion_function(text, state);
}

char **completion(const char *text, int start, int end) {
    // If completing the command (start == 0), use the command generator
    if (start == 0) {
        rl_attempted_completion_over = 1;
        return rl_completion_matches(text, command_generator);
    }

    // Otherwise, assume completing a path (file argument)
    return rl_completion_matches(text, path_generator);
}