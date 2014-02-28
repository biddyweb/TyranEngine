#include <tyran_engine/resource/resource_definitions.h>
#include <tyran_engine/resource/resource_definition.h>

void nimbus_resource_definitions_init(nimbus_resource_definitions* self, struct tyran_memory* memory)
{
	nimbus_array_init(&self->definitions_array, memory, sizeof(nimbus_resource_definition), 512);
	self->definitions = self->definitions_array.array;
	self->count = 0;
}

void nimbus_resource_definitions_free(nimbus_resource_definitions* self)
{
	self->definitions = 0;
	nimbus_array_destroy(&self->definitions_array);
}

struct nimbus_resource_definition* nimbus_resource_definitions_find(nimbus_resource_definitions* self, nimbus_resource_type_id type_id)
{
	for (int i = 0; i < self->count; ++i) {
		nimbus_resource_definition* definition = &self->definitions[i];
		if (definition->resource_type_id == type_id) {
			return definition;
		}
	}

	return 0;
}

struct nimbus_resource_definition* nimbus_resource_definitions_create(nimbus_resource_definitions* self, nimbus_resource_type_id type_id, size_t struct_size, const char* debug_string)
{
	nimbus_resource_definition* definition = &self->definitions[self->count++];
	definition->resource_type_id = type_id;
	definition->struct_size = struct_size;
	definition->debug_string = debug_string;
	return definition;
}
