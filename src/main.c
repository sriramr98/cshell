#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array_list.h"

char* EXIT_COMMAND = "exit";
char* ECHO_COMMAND = "echo";

typedef struct {
  char* command;
  ArrayList* inputs;
} Command;

void free_command(Command* cmd) {
  free(cmd->command);
  free_array_list(cmd->inputs);
  free(cmd);
}

Command* parse_command(char* command) {
  Command* cmd = malloc(sizeof(Command));
  cmd->inputs = new_array_list(10);

  char* token = strtok(command, " ");

  while (token != NULL) {
    int length = strlen(token);
    if (cmd->command == NULL) {
      cmd->command = malloc(sizeof(char) * length);
      strcpy(cmd->command, token);
    } else {
      add_array_list(cmd->inputs, token);
    }
    token = strtok(NULL, " ");
  }

  return cmd;
}

void perform_echo(Command* cmd) {
  if (cmd->inputs == NULL || cmd->inputs->size == 0) {
    return;
  }
  for (int i=0; i<cmd->inputs->size; i++) {
    printf("%s", get_element_from_list(cmd->inputs, i));
    if (i != cmd->inputs->size-1) {
      printf(" ");
    }
  }

  printf("\n");
}

// Captures the user's command in the "command" variable
char command[1024];

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

    Command* cmd = parse_command(command);
    if (cmd == NULL) {
      continue;
    }

    if (strcmp(cmd->command, EXIT_COMMAND) == 0) {
      return 0;
    }

    if (strcmp(cmd->command, ECHO_COMMAND) == 0) {
      perform_echo(cmd);
      continue;
    }

    printf("%s: command not found\n", command);

    free_command(cmd);
  }
  
  return 0;
}