typedef struct {
  char** inputs;
  int capacity;
  int size;
} ArrayList;

ArrayList* new_array_list(int capacity);
void add_array_list(ArrayList* list, char* element);
char* get_element_from_list(ArrayList* list, int index);
void free_array_list(ArrayList* list);
int array_list_size(ArrayList* list);
int array_list_capacity(ArrayList* list);