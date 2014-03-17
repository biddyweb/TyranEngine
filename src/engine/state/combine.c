#include <tyran_engine/state/combine.h>
#include <tyranscript/tyran_memory.h>
#include <tyran_engine/state/component_definition.h>
#include <tyran_engine/state/event_component_header.h>

#include "state.h"

static int NIMBUS_COMBINE_MAX_COMPONENT_COUNT = 32;

void nimbus_combine_init(nimbus_combine* self, nimbus_state* state)
{
	self->state = state;
	self->components_count = 0;
}

nimbus_component* nimbus_combine_create_component(nimbus_combine* self, tyran_symbol name, const nimbus_component_definition* component_definition)
{
	TYRAN_ASSERT(self->components_count < NIMBUS_COMBINE_MAX_COMPONENT_COUNT, "Too many components");
	nimbus_component* component = &self->components[self->components_count++];

	nimbus_event_component_header* component_data = nimbus_state_create_component(self->state, component_definition);

	size_t struct_size = component_definition->struct_size;

	nimbus_component_init(component, self, name, component_definition, component_data, struct_size);

	return component;
}

nimbus_component* nimbus_combine_component_from_name(nimbus_combine* self, tyran_symbol name)
{
	for (int i=0; i<self->components_count; ++i) {
		nimbus_component* component = &self->components[i];
		if (tyran_symbol_equal(&component->component_name, &name)) {
			return component;
		}
	}
	TYRAN_ERROR("Couldn't find component #%d", name.hash);
	return 0;
}

