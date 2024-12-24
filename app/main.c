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

    // if user wants to exit
    if (!strcmp(input, "exit 0"))
      exit(0);

    printf("%s: command not found\n", input);
  }

  return 0;
}
