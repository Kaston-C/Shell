#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include "parser.h"

char** process_command(char *input) {
    size_t len = strlen(input);
    char temp[MY_SHELL_MAX_INPUT];
    char token[MY_SHELL_MAX_INPUT];
    char **args = malloc(MY_SHELL_MAX_INPUT * sizeof(char*));
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
    while (j > 0 && isspace(temp[j - 1])) {
        j--;
    }
    temp[j] = '\0';

    strcpy(input, temp);

    return args;
}

Pipeline parse_input(char *input) {
    Pipeline pipeline;
    pipeline.commands = malloc(sizeof(Command) * MY_SHELL_MAX_INPUT);
    pipeline.count = 0;

    char *saveptr;
    char *segment = strtok_r(input, "|", &saveptr);

    while (segment) {
        char segment_copy[MY_SHELL_MAX_INPUT];
        strncpy(segment_copy, segment, MY_SHELL_MAX_INPUT);

        Command cmd;
        cmd.argv = process_command(segment);

        pipeline.commands[pipeline.count++] = cmd;
        segment = strtok_r(NULL, "|", &saveptr);
    }

    return pipeline;
}