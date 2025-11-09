#ifndef PROGRAM_H
#define PROGRAM_H

#include "array_list.h"

typedef struct {
  char* name;
  char* path;
} Program;

Program* new_program(char* name, char* path);

void delete_program(Program* prog);

Program* find_program(char* name);

#endif // PROGRAM_H