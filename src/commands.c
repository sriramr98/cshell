#include "commands.h"
#include "program.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// Global variable definitions
char* EXIT_COMMAND = "exit";
char* ECHO_COMMAND = "echo";
char* TYPE_COMMAND = "type";
char* CD_COMMAND = "cd";
char* PWD_COMMAND = "pwd";

void changeDir(char* path) {
  int resp = chdir(path);
  if (resp == -1) {
    printf("cd: %s: No such file or directory\n", path);
  }
}

void free_command(Command* cmd) {
  free(cmd->command);
  free_array_list(cmd->inputs);
  free(cmd);
}

Command* parse_command(char* command) {
  Command* cmd = malloc(sizeof(Command));
  cmd->command = NULL;  // Initialize to NULL
  cmd->inputs = new_array_list(10);

  char* tmp = malloc(sizeof(char) * (strlen(command) + 1));
  strcpy(tmp, command);
  char* token = strtok(tmp, " ");

  while (token != NULL) {
    int length = strlen(token);
    if (cmd->command == NULL) {
      cmd->command = malloc(sizeof(char) * (length + 1));
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
    printf("\n");
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

void perform_type(Command* cmd) {
  if (cmd->inputs->size == 0) {
    return;
  }

  char* param = get_element_from_list(cmd->inputs, 0);
  if (param == NULL) {
    return;
  }

  if ((strcmp(param, EXIT_COMMAND) == 0) || (strcmp(param, ECHO_COMMAND) == 0) || (strcmp(param, TYPE_COMMAND) == 0) || (strcmp(param, PWD_COMMAND) == 0) ) {
    printf("%s is a shell builtin\n", param);
  } else { 
    Program* program = find_program(param);
    if (program == NULL) {
      printf("%s: not found\n", param);
    } else {
      printf("%s is %s\n", param, program->path);
    }

    delete_program(program);
  }
}

void perform_cd(Command* cmd) {
  if (cmd->inputs->size == 0) {
    //TODO: I think just `cd` should move to root of the OS??
    return; 
  }

  char* path = get_element_from_list(cmd->inputs, 0);
  if (path == NULL) {
    //TODO: I think it's same scenario as above?
    return;
  }

  if (strcmp(path, "~") == 0) {
    // CD to root
    char* rootDir = getenv("HOME");
    if (rootDir == NULL) return;
 
    changeDir(rootDir);
    return;
  }

  changeDir(path); 
}

void perform_pwd(Command* cmd) {
  char* cwd = malloc(1024);
  if (getcwd(cwd, 1024) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("pwd");
  }
  free(cwd);
}