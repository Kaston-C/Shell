#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "parser.h"

void handle_command(char *input);
int execute_single_command(char *input, char **args);
void execute_pipeline(char *input, Pipeline pipeline);

#endif //EXECUTOR_H
