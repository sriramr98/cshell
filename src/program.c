#include "program.h"

#include<stdlib.h>
#include<dirent.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>

Program* new_program(char* name, char* path) {
  Program* prog = malloc(sizeof(Program));
  if (prog == NULL) return NULL;
  
  // Allocate memory for name and path strings
  prog->name = malloc(strlen(name) + 1);
  prog->path = malloc(strlen(path) + 1);
  
  if (prog->name == NULL || prog->path == NULL) {
    free(prog->name);
    free(prog->path);
    free(prog);
    return NULL;
  }
  
  strcpy(prog->name, name);
  strcpy(prog->path, path);

  return prog;
}

void delete_program(Program* prog) {
  if (prog == NULL) return;
  free(prog->name);
  free(prog->path);
  free(prog);
}

Program* find_program(ArrayList* paths, char* name) {
    if (paths == NULL) return NULL;
    if (paths->size == 0) return NULL;
    
    DIR*           FD;        /* represent the directory */
    struct dirent* in_file;   /* represent the file */

    for (int i=0; i< paths->size; i++) {
        char* path = get_element_from_list(paths, i);
        
        if (path == NULL) continue;

        FD = opendir(path);
        if (FD == NULL) {
            //TODO? Any output here??
            continue;
        }

        while((in_file = readdir(FD))) {
            // If `in_file` is a file and name matches, create program
            if (in_file->d_type == DT_REG && strcmp(in_file->d_name, name) == 0) {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, in_file->d_name);
              
                if (access(full_path, X_OK) != 0) continue;

                Program* prog = new_program(in_file->d_name, full_path);
                
                closedir(FD);
                return prog;
            }
        }
        closedir(FD);
    }
    
    return NULL; // No executable program found

}