#ifndef nimbus_component_array_h
#define nimbus_component_array_h

#include "../script/bit_array.h"

struct nimbus_event_component_header;

typedef struct nimbus_component_array {
	void* data;
	size_t struct_size;
	nimbus_bit_array bit_array;
	int max_count;
	const struct nimbus_component_definition* definition;
} nimbus_component_array;

void nimbus_component_array_init(nimbus_component_array* self, struct tyran_memory* memory, const struct nimbus_component_definition* definition, int max_count);
void nimbus_component_array_free(nimbus_component_array* self);
struct nimbus_event_component_header* nimbus_component_array_create_component(nimbus_component_array* self, int* index);
void nimbus_component_array_delete_component(nimbus_component_array* self, int index);

#endif
