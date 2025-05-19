#ifndef BUILTIN_H
#define BUILTIN_H

#define MAX_PATH 1024

int execute_builtin_command(char *input, char **args, FILE *output_stream);
int is_builtin_command(const char* cmd);

#endif //BUILTIN_H
