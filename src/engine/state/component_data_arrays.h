#ifndef nimbus_component_arrays_h
#define nimbus_component_arrays_h

#include "component_data_array.h"

struct tyran_memory;
struct nimbus_component_definition;

typedef struct nimbus_component_arrays {
	nimbus_component_array tracks[32];
	int tracks_count;
	struct tyran_memory* memory;
} nimbus_component_arrays;

void nimbus_component_arrays_init(nimbus_component_arrays* self, struct tyran_memory* memory, int max_count);
nimbus_component_array* nimbus_component_arrays_add(nimbus_component_arrays* self, struct nimbus_component_definition* definition);
nimbus_component_array* nimbus_component_arrays_array_from_definition(nimbus_component_arrays* self, const struct nimbus_component_definition* definition);

#endif
