#include "component_data_arrays.h"
#include "component_data_array.h"

#include <tyran_engine/state/component_definition.h>

void nimbus_component_arrays_init(nimbus_component_arrays* self, struct tyran_memory* memory, int max_count)
{
	TYRAN_ASSERT(max_count <= 32, "can't hold that many component arrays");
	self->memory = memory;
	self->tracks_count = 0;
}

nimbus_component_array* nimbus_component_arrays_add(nimbus_component_arrays* self, const nimbus_component_definition* definition)
{
	TYRAN_ASSERT(self->tracks_count <= 32, "hit max component arrays");
	TYRAN_ASSERT(definition != 0, "can not add null definition");
	nimbus_component_array* array = &self->tracks[self->tracks_count++];
	nimbus_component_array_init(array, self->memory, definition, 256);

	return array;
}

nimbus_component_array* nimbus_component_arrays_array_from_definition(nimbus_component_arrays* self, const struct nimbus_component_definition* definition)
{
	for (int i=0; i<self->tracks_count; ++i) {
		nimbus_component_array* array = &self->tracks[i];
		if (array->definition == definition) {
			return array;
		}
	}
	
	TYRAN_ERROR("NO DEFINITION FOUND!");
	return 0;
}