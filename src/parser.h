#ifndef PARSER_H
#define PARSER_H

#define MY_SHELL_MAX_INPUT 1024

typedef struct {
    char **argv;
} Command;

typedef struct {
    Command *commands;
    int count;
} Pipeline;

char** process_command(char *input);
Pipeline parse_input(char *input);

#endif //PARSER_H
