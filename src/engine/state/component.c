#include <tyran_engine/state/component.h>
#include <tyranscript/tyran_log.h>
#include <tyran_engine/state/component_definition.h>

void nimbus_component_init(nimbus_component* self, struct nimbus_combine* owner_combine, const struct nimbus_component_definition* definition, u8t* data, size_t data_octet_size)
{
	self->component_definition = definition;
	self->component_data = data;
	self->parent_combine = owner_combine;
	TYRAN_ASSERT(data_octet_size == definition->struct_size, "Illegal data size allocated!");
}

void nimbus_component_move(nimbus_component* self, u8t* new_data, size_t data_octet_size)
{
	TYRAN_ASSERT(data_octet_size == self->component_definition->struct_size, "Wrong allocation size");
	tyran_memcpy_octets(new_data, self->component_data, self->component_definition->struct_size);
	self-<component_data = new_data;
}
