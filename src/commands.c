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
char* extract_argument(char* source, int argStart, int argEnd, int argLen, char quoteType) {
  // printf("ArgStart: %d, ArgEnd: %d, ArgLen: %d\n", argStart, argEnd, argLen);
  char* arg = malloc(sizeof(char) * (argLen + 1));
  if (arg == NULL) {
    return NULL;
  }

  int j = argStart;
  int argIdx = 0;
  int isInsideQuote = -1;

  while (j < argEnd) {
    if (source[j] == '\\' && quoteType == '\0') {
      j++; // skip the escape character
      arg[argIdx] = source[j]; // the next character is included regardless of what it
    } else if (source[j] == quoteType) {
      // Skip quotes. Empty block.
      j++;
      continue;
    } else {
      arg[argIdx] = source[j];
    }

    argIdx++;
    j++;
  }

  arg[argIdx] = '\0';

  return arg;

}

void print_cmd(Command* cmd) {
  printf("Command: %s\n", cmd->command);
  for (int i=0; i<cmd->inputs->size; i++) {
    printf("Arg %d - %s\n", i+1, get_element_from_list(cmd->inputs, i));
  }
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
  char quoteType = '\0';
  int argLen = 0;

  // printf("Staring parse arguments..\n");

  // Parse arguments
  while (i < charCount) {
    if (tmp[i] == '\\') {
      if (isInsideQuote == 1) {
        // If escape character is inside the quote, include it in the result string
        i++;
        argLen++;
      } else {
        // For escape characters, consider this and the next character valid
        i+=2;
        argLen++;
      }
    } else if (tmp[i] == '\'') {
      // printf("Found quotes at %d\n", i);
      if (quoteType == '\0') {
        // Starting of a quote as param
        quoteType = '\'';
        isInsideQuote = 1;
      } else if (quoteType == '\'') {
        isInsideQuote = isInsideQuote == 1 ? -1 : 1;
      } else {
        argLen++;
      }
      i++;
    } else if (tmp[i] == '"') {
      if (quoteType == '\0') {
        quoteType = '"';
        isInsideQuote = 1;
      } else if (quoteType == '"') {
        isInsideQuote = isInsideQuote == 1 ? -1 : 1;
      }
      i++;
    } else if (tmp[i] == ' ') {
      // printf("Found non quote %c at %d, IsInsideQuote: %d\n", tmp[i], i, isInsideQuote);
      if (isInsideQuote == 1) {
        // This space is enclosed between quotes, so include this and continue reading
        i++;
        argLen++;
      } else {
        // This marks the end of a parameter
        int argEnd = i;

        char* arg = extract_argument(tmp, argStart, argEnd, argLen, quoteType);
                
        add_array_list(cmd->inputs, arg);
        free(arg);

        i = argEnd + 1;

        isInsideQuote = -1;
        argLen = 0;
        quoteType = '\0';

        // Skip all whitespaces
        // printf("Skipping char count from %d\n", i);
        while (i<charCount && tmp[i] == ' ') {
          i++;
        }

        argStart = i;
      }
    } else {
      // printf("Found non quote non space at %c in %d\n", tmp[i], i);
      i++;
      argLen++;
    }
  }

  if (argStart < i) {
    // Parse last parameter
    char* arg = extract_argument(tmp, argStart, i, argLen, quoteType);
    add_array_list(cmd->inputs, arg);
    free(arg);
  }

  free(tmp);
  // print_cmd(cmd);
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