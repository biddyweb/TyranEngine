#include "track_info.h"
#include "bit_array.h"
#include <tyranscript/tyran_object.h>

void nimbus_track_info_init(nimbus_track_info* self, struct tyran_memory* memory, tyran_symbol type_symbol)
{
	self->type_symbol = type_symbol;
	const static size_t MAX_OBJECTS = 1024;
	nimbus_bit_array_init(&self->bit_array, memory, MAX_OBJECTS);
	self->objects = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, tyran_object*, MAX_OBJECTS);
}

void nimbus_track_info_free(nimbus_track_info* self)
{
	tyran_free(self->objects);
	nimbus_bit_array_free(&self->bit_array);
}

int nimbus_track_info_get_free_index(nimbus_track_info* self, tyran_object* o)
{
	int index = nimbus_bit_array_reserve_free_index(&self->bit_array);
	TYRAN_ASSERT(!self->objects[index], "Already an object at this position");
	self->objects[index] = o;
	TYRAN_OBJECT_RETAIN(self->objects[index]);
	return index;
}

void nimbus_track_info_delete_index(nimbus_track_info* self, int index)
{
	tyran_object* o = self->objects[index];
	if (o) {
		tyran_object_release(o);
		self->objects[index] = 0;
	}
	nimbus_bit_array_delete_index(&self->bit_array, index);
}
