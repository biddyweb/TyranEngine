#include "state.h"
#include "combines.h"
#include "combine.h"

#include "component_definition.h"
#include "event_component_header.h"

void nimbus_state_init(nimbus_state* self, struct tyran_memory* memory, const struct nimbus_component_definition* definitions, int definitions_count)
{
	self->definitions = definitions;
	self->definitions_count = definitions_count;
	nimbus_component_arrays_init(&self->arrays, memory, 32);
	TYRAN_ASSERT(definitions_count > 0, "Must have definitions!");
	for (int i=0; i<definitions_count; ++i) {
		nimbus_component_arrays_add(&self->arrays, &definitions[i]);
	}
	nimbus_combines_init(&self->combines, memory, 256);
}

struct nimbus_event_component_header* nimbus_state_create_component(nimbus_state* self, const struct nimbus_component_definition* definition)
{
	nimbus_component_array* array = nimbus_component_arrays_array_from_definition(&self->arrays, definition);
	int index;
	nimbus_event_component_header* header = nimbus_component_array_create_component(array, &index);
	return header;
}

void nimbus_state_clone(nimbus_state* self, const nimbus_state* source)
{

}

nimbus_combine* nimbus_state_create_combine(nimbus_state* self)
{
	nimbus_combine* combine = nimbus_combines_create(&self->combines, self);
	
	return combine;
}
