#ifndef nimbus_resource_definitions_h
#define nimbus_resource_definitions_h

#include <tyran_engine/resource/type_id.h>
#include <tyran_engine/array/array.h>

struct tyran_memory;
struct nimbus_resource_definition;

typedef struct nimbus_resource_definitions {
	nimbus_array definitions_array;
	struct nimbus_resource_definition* definitions;
	int count;
} nimbus_resource_definitions;

void nimbus_resource_definitions_init(nimbus_resource_definitions* self, struct tyran_memory* memory);
void nimbus_resource_definitions_free(nimbus_resource_definitions* self);

struct nimbus_resource_definition* nimbus_resource_definitions_find(nimbus_resource_definitions* self, nimbus_resource_type_id type_id);
struct nimbus_resource_definition* nimbus_resource_definitions_create(nimbus_resource_definitions* self, nimbus_resource_type_id type_id, size_t struct_size, const char* debug_string);

#endif
