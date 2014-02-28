#include "component_data_arrays.h"
#include "component_data_array.h"

#include <tyran_engine/state/component_definition.h>

void nimbus_component_arrays_init(nimbus_component_arrays* self, struct tyran_memory* memory, int max_count)
{
	TYRAN_ASSERT(max_count <= 32, "can't hold that many component arrays");
	self->memory = memory;
	self->tracks_count = 0;
}

nimbus_component_array* nimbus_component_arrays_add(nimbus_component_arrays* self, nimbus_component_definition* definition)
{
	TYRAN_ASSERT(self->tracks_count <= 32, "hit max component arrays");
	nimbus_component_array* array = &self->tracks[self->tracks_count++];
	nimbus_component_array_init(array, self->memory, definition->struct_size, 256);

	return array;
}

nimbus_component_array* nimbus_component_arrays_array_from_definition(nimbus_component_arrays* self, const struct nimbus_component_definition* definition)
{
	return 0;
}