#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* EXIT_COMMAND = "exit";

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");

    // Captures the user's command in the "command" variable
    char command[1024];
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = '\0';

    if (strncmp(command, EXIT_COMMAND, strlen(EXIT_COMMAND)) == 0) {
      return 0;
    }

    printf("%s: command not found\n", command);

  }
  
  return 0;
}
