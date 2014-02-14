#ifndef nimbus_combine_h
#define nimbus_combine_h

#include "component.h"

struct tyran_memory;

typedef struct nimbus_combine {
	nimbus_component components[32];
	int components_count;

	u8t* components_memory;
	int used_memory_octet_count;
	int allocated_memory_octet_count;


	struct tyran_memory* memory;
} nimbus_combine;

void nimbus_combine_init(nimbus_combine* self, struct tyran_memory* memory);
nimbus_component* nimbus_combine_add_component(nimbus_combine* self, const struct nimbus_component_definition* component_definition);
void nimbus_combine_clone(const nimbus_combine* self, nimbus_combine* clone, struct tyran_memory* memory);

#endif
