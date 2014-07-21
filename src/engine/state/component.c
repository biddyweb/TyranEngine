#include <tyran_engine/state/component.h>
#include <tyranscript/tyran_log.h>
#include <tyran_engine/state/component_definition.h>

void nimbus_component_init(nimbus_component* self, struct nimbus_combine* owner_combine, tyran_symbol component_name, const struct nimbus_component_definition* definition, struct nimbus_event_component_header* data, size_t data_octet_size)
{
	self->component_definition = definition;
	self->component_data = data;
	self->parent_combine = owner_combine;
	self->component_name = component_name;
	TYRAN_ASSERT(data_octet_size == definition->struct_size, "Illegal data size allocated!");
}

void nimbus_combine_component_add_extra_reference(nimbus_component* self, void* p, nimbus_resource_id resource_id, tyran_symbol component_name, tyran_symbol property_name)
{
	nimbus_extra_reference* ref = &self->extra_references[self->extra_references_count++];
	ref->pointer = p;
	ref->resource_id = resource_id;
	ref->component_name = component_name;
	ref->property_name = property_name;
}
