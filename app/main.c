#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  // REPL loop
  while (1) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);
    // set newline to null terminator
    input[strlen(input) - 1] = '\0';

    if (strncmp(input, "exit 0", 6) == 0) {
        exit(0);
    } else if (strncmp(input, "echo ", 5) == 0) {
        printf("%s\n", input + 5);
    } else if (strncmp(input, "type ", 5) == 0) {
        if (strncmp(input + 5, "exit", 4) == 0) {
            printf("exit is a shell builtin");
        } else if (strncmp(input + 5, "echo", 4) == 0) {
            printf("echo is a shell builtin");
        } else if (strncmp(input + 5, "type", 4) == 0) {
            printf("type is a shell builtin");
        } else {
            printf("%s: not found\n", input + 5);
        }
    } else {
        printf("%s: command not found\n", input);
    }
  }

  return 0;
}
