#ifndef nimbus_array_h
#define nimbus_array_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef struct nimbus_array {
	void* array;
	int allocated_count;
	size_t struct_size;
} nimbus_array;

void nimbus_array_init(nimbus_array* self, struct tyran_memory* memory, size_t struct_size, int max_count);
void nimbus_array_destroy(nimbus_array* self);

#endif
