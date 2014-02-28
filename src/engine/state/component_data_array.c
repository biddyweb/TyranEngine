#include "component_data_array.h"
#include "component_definition.h"
#include <tyran_engine/state/event_component_header.h>

void nimbus_component_array_init(nimbus_component_array* self, struct tyran_memory* memory, const struct nimbus_component_definition* definition, int max_count)
{
	self->struct_size = definition->struct_size;
	self->definition = definition;
	self->max_count = max_count;
	self->data = TYRAN_MEMORY_CALLOC(memory, self->struct_size * max_count, "component_array");
	nimbus_bit_array_init(&self->bit_array, memory, max_count);
}

void nimbus_component_array_free(nimbus_component_array* self)
{
	TYRAN_MEMORY_FREE(self->data);
	nimbus_bit_array_free(&self->bit_array);
}

nimbus_event_component_header* nimbus_component_array_create_component(nimbus_component_array* self, int* return_index)
{
	int index = nimbus_bit_array_reserve_free_index(&self->bit_array);
	TYRAN_ASSERT(index >= 0, "No indexes left");
	*return_index = index;
	nimbus_event_component_header* component_data = (nimbus_event_component_header*)(((u8t*)self->data) + self->struct_size * index);
	component_data->is_used = TYRAN_TRUE;
	return component_data;
}

void nimbus_component_array_delete_component(nimbus_component_array* self, int index)
{
	void* component_data = ((u8t*)self->data) + self->struct_size * index;
	tyran_mem_clear(component_data, self->struct_size);
	nimbus_bit_array_delete_index(&self->bit_array, index);
}
