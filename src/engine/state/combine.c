#include <tyran_engine/state/combine.h>
#include <tyranscript/tyran_memory.h>
#include <tyran_engine/state/component_definition.h>

static int NIMBUS_COMBINE_MAX_COMPONENT_COUNT = 32;

void nimbus_combine_init(nimbus_combine* self, struct tyran_memory* memory)
{
	self->memory = memory;
	self->components_count = 0;
	self->components_memory = 0;
	self->used_memory_octet_count = 0;
}

void realloc_components(nimbus_combine* self, int minimum_octet_size)
{
	int new_size = self->allocated_memory_octet_count + minimum_octet_size + (self->allocated_memory_octet_count * 2);
	u8t* new_memory = TYRAN_MEMORY_CALLOC(self->memory, new_size, "components memory");
	u8t* new_component_memory_pointer = new_memory;
	self->allocated_memory_octet_count = 0;
	for (int component_index=0; component_index<self->components_count; ++component_index) {
		nimbus_component* component = &self->components[component_index];
		int component_data_octet_size = component->component_definition->struct_size;
		nimbus_component_move(component, new_component_memory_pointer, component_data_octet_size);
		new_component_memory_pointer += component_data_octet_size;
		self->allocated_memory_octet_count += component_data_octet_size;
	}
	TYRAN_MEMORY_FREE(self->components_memory);
	self->components_memory = new_memory;
}

nimbus_component* nimbus_combine_add_component(nimbus_combine* self, const nimbus_component_definition* component_definition)
{
	TYRAN_ASSERT(self->components_count < NIMBUS_COMBINE_MAX_COMPONENT_COUNT, "Too many components");
	nimbus_component* component = &self->components[self->components_count++];
	int memory_left = self->allocated_memory_octet_count - self->used_memory_octet_count;
	const int struct_size = component_definition->struct_size;
	if (struct_size > memory_left) {
		realloc_components(self, component_definition->struct_size);
	}

	u8t* p = self->components_memory + self->used_memory_octet_count;
	nimbus_component_init(component, self, component_definition, p, struct_size);

	return component;
}

void nimbus_combine_clone(const nimbus_combine* self, nimbus_combine* clone, struct tyran_memory* memory)
{
	*clone = *self;
	clone->components_memory = TYRAN_MEMORY_CALLOC(self->memory, self->allocated_memory_octet_count, "components clone");
	tyran_memcpy_octets(clone->components_memory, self->components_memory, self->allocated_memory_octet_count);
}
