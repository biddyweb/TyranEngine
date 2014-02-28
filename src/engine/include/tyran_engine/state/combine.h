#ifndef nimbus_combine_h
#define nimbus_combine_h

#include "component.h"

struct tyran_memory;
struct nimbus_state;

typedef struct nimbus_combine {
	nimbus_component components[32];
	int components_count;

	struct nimbus_state* state;

	struct tyran_memory* memory;
} nimbus_combine;

void nimbus_combine_init(nimbus_combine* self, struct nimbus_state* state);
nimbus_component* nimbus_combine_create_component(nimbus_combine* self, const struct nimbus_component_definition* component_definition);

//void nimbus_combine_clone(const nimbus_combine* self, nimbus_combine* clone, struct tyran_memory* memory);

#endif