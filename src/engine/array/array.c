#include <tyran_engine/array/array.h>
#include <tyranscript/tyran_memory.h>

void nimbus_array_init(nimbus_array* self, struct tyran_memory* memory, size_t struct_size, int max_count)
{
	self->array = TYRAN_MEMORY_CALLOC(memory, max_count * struct_size, "nimbus_array");
	self->allocated_count = max_count;
	self->struct_size = struct_size;
}

void nimbus_array_destroy(nimbus_array* self)
{
	TYRAN_MEMORY_FREE(self->array);
	self->allocated_count = 0;
	self->struct_size = 0;
}
