#include <stdlib.h>
#include <string.h>
#include "array_list.h"

ArrayList* new_array_list(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }
    
    ArrayList* list = malloc(sizeof(ArrayList));
    if (list == NULL) {
        return NULL;
    }
    
    list->capacity = capacity;
    list->size = 0;
    list->inputs = malloc(capacity * sizeof(char*));
    
    if (list->inputs == NULL) {
        free(list);
        return NULL;
    }

    return list;
}

void add_array_list(ArrayList* list, char* element) {
    if (list == NULL || element == NULL) {
        return;
    }
    
    // Check if we need to resize
    if (list->size >= list->capacity) {
        // Double the capacity
        int new_capacity = list->capacity * 2;
        char** new_inputs = realloc(list->inputs, new_capacity * sizeof(char*));
        
        if (new_inputs == NULL) {
            return; // Failed to resize, could not add element
        }
        
        list->inputs = new_inputs;
        list->capacity = new_capacity;
    }
    
    // Allocate memory for the string copy
    list->inputs[list->size] = malloc((strlen(element) + 1) * sizeof(char));
    if (list->inputs[list->size] == NULL) {
        return;
    }
    
    // Copy the string
    strcpy(list->inputs[list->size], element);
    list->size++;
}

char* get_element_from_list(ArrayList* list, int index) {
    if (list == NULL || index < 0 || index >= list->size) {
        return NULL;
    }
    
    return list->inputs[index];
}

void free_array_list(ArrayList* list) {
    if (list == NULL) {
        return;
    }
    
    // Free all allocated strings
    for (int i = 0; i < list->size; i++) {
        if (list->inputs[i] != NULL) {
            free(list->inputs[i]);
        }
    }
    
    // Free the array of pointers
    free(list->inputs);
    
    // Free the list structure itself
    free(list);
}

int array_list_size(ArrayList* list) {
    if (list == NULL) {
        return -1;
    }
    return list->size;
}

int array_list_capacity(ArrayList* list) {
    if (list == NULL) {
        return -1;
    }
    return list->capacity;
}