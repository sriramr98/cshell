#ifndef COMMANDS_H
#define COMMANDS_H

#include "array_list.h"

typedef struct {
  char* command;
  ArrayList* inputs;
} Command;

// External variable declarations
extern char* EXIT_COMMAND;
extern char* ECHO_COMMAND;
extern char* TYPE_COMMAND;
extern char* CD_COMMAND;
extern char* PWD_COMMAND;

// Function declarations
Command* parse_command(char* command);
void free_command(Command* cmd);
void perform_echo(Command* cmd);
void perform_type(Command* cmd);
void perform_cd(Command* cmd);
void perform_pwd(Command* cmd);

#endif // COMMANDS_H