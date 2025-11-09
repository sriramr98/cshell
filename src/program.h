#include "array_list.h"

typedef struct {
  char* name;
  char* path;
} Program;

Program* new_program(char* name, char* path);

void delete_program(Program* prog);

Program* find_program(ArrayList* paths, char* name);