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

// source is the string from where we need to extract the argument enclosed between argStart and argEnd.
// there will be exactly `quoteCount` quotes in between.
char* extract_argument(char* source, int argStart, int argEnd, int quoteCount) {
  // printf("ArgStart: %d, ArgEnd: %d, QuoteCount: %d\n", argStart, argEnd, quoteCount);
  int argLen = argEnd - argStart - quoteCount; // The argument should exclude quotes.

  char* arg = malloc(sizeof(char) * argLen);
  if (arg == NULL) {
    return NULL;
  }

  int argIdx = 0;
  for (int j = argStart; j < argEnd; j++) {
    if (source[j] == '\'' || source[j] == '"') {
      // printf("Skipping quotes\n");
      continue;
    } else {
      // printf("ArgStart: %d, J: %d, ArgIdx: %d\n", argStart, j, argIdx);
      arg[argIdx] = source[j];
      argIdx++;
    }
  }

  arg[argIdx] = '\0';

  printf("Found argument %s\n", arg);

  return arg;

}

Command* parse_command(char* command) {
  Command* cmd = malloc(sizeof(Command));
  cmd->command = NULL;
  cmd->inputs = new_array_list(10);

  int charCount = strlen(command);
  char* tmp = malloc(sizeof(char) * (charCount + 1));
  strcpy(tmp, command);

  int i = 0;
  
  // Skip leading whitespace
  while (i < charCount && tmp[i] == ' ') {
    i++;
  }
  
  if (i >= charCount) {
    // Empty command
    free(tmp);
    return cmd;
  }

  // Extract command name (first word)
  int cmdStart = i;
  while (i < charCount && tmp[i] != ' ') {
    i++;
  }
  
  int cmdLen = i - cmdStart;
  cmd->command = malloc(sizeof(char) * (cmdLen + 1));
  strncpy(cmd->command, tmp + cmdStart, cmdLen);
  cmd->command[cmdLen] = '\0';

  // Skip whitespace
  while (i < charCount && tmp[i] == ' ') {
    i++;
  }

  int argStart = i;

  int isInsideQuote = -1;
  int quoteCount = 0;

  // printf("Staring parse arguments..\n");

  // Parse arguments
  while (i < charCount) {
 
    if (tmp[i] == '\'') {
      // printf("Found quotes at %d\n", i);
      if (isInsideQuote == 1) {
        isInsideQuote = -1;
      } else {
        isInsideQuote = 1;
      }
      quoteCount++;
      i++;
      continue;
    }


    if (tmp[i] == ' ') {
      // printf("Found non quote %c at %d\n", tmp[i], i);
      if (isInsideQuote == 1) {
        // This space is enclosed between quotes, so include this and continue reading
        i++;
      } else {
        // This marks the end of a parameter
        int argEnd = i;

        char* arg = extract_argument(tmp, argStart, argEnd, quoteCount);
                
        add_array_list(cmd->inputs, arg);
        free(arg);

        i = argEnd + 1;
        argStart = i;

        isInsideQuote = -1;
        quoteCount = 0;
      }
    } else {
      // printf("Found non quote non space at %c in %d\n", tmp[i], i);
      i++;
    }
  }

  if (argStart < i) {
    // Parse last parameter
    char* arg = extract_argument(tmp, argStart, i, quoteCount);
    add_array_list(cmd->inputs, arg);
    free(arg);
  }

  free(tmp);
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