#ifndef nimbus_state_h
#define nimbus_state_h

#include "component_data_arrays.h"
#include "combines.h"

struct tyran_memory;
struct nimbus_component_definition;
struct nimbus_combine;
struct nimbus_event_component_header;

typedef struct nimbus_state {
	nimbus_component_arrays arrays;
	nimbus_combines combines;
	const struct nimbus_component_definition* definitions;
	int definitions_count;
} nimbus_state;

void nimbus_state_init(nimbus_state* self, struct tyran_memory* memory, const struct nimbus_component_definition* definitions, int definitions_count);

struct nimbus_combine* nimbus_state_create_combine(nimbus_state* self);

struct nimbus_event_component_header* nimbus_state_create_component(nimbus_state* self, const struct nimbus_component_definition* definition);
void nimbus_state_clone(nimbus_state* self, const nimbus_state* source);

#endif
