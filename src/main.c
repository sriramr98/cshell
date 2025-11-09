#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "program.h"
#include "commands.h"

// Captures the user's command in the "command" variable
char command[1024];

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");

    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "") == 0) {
      continue;
    }

    Command* cmd = parse_command(command);
    if (cmd == NULL) {
      continue;
    }

    if (strcmp(cmd->command, EXIT_COMMAND) == 0) {
      free_command(cmd);
      return 0;
    }

    if (strcmp(cmd->command, ECHO_COMMAND) == 0) {
      perform_echo(cmd);
      free_command(cmd);
      continue;
    }

    if (strcmp(cmd->command, TYPE_COMMAND) == 0) {
      perform_type(cmd);
      free_command(cmd);
      continue;
    }

    if (strcmp(cmd->command, PWD_COMMAND) == 0) {
      perform_pwd(cmd);
      free_command(cmd);
      continue;
    }

    if (strcmp(cmd->command, CD_COMMAND) == 0) {
      perform_cd(cmd);
      free_command(cmd);
      continue;
    }

    Program* prg = find_program(cmd->command);

    if (prg == NULL) {
      printf("%s: command not found\n", command);
      free_command(cmd);
    } else {
      system(command);
    }
  }
  
  return 0;
}