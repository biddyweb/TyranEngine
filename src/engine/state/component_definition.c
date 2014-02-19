#include <tyran_engine/state/component_definition.h>
#include <tyranscript/tyran_symbol_table.h>

void nimbus_component_definition_init(nimbus_component_definition* self, struct tyran_symbol_table* symbol_table, const char* debug_name, u8t event_type_id, size_t struct_size)
{
	self->symbol_table = symbol_table;
	self->debug_name = debug_name;
	self->event_type_id = event_type_id;
	self->properties_count = 0;
	self->struct_size = struct_size;
	//tyran_symbol type_symbol;
}

void nimbus_component_definition_add_property(nimbus_component_definition* self, const char* name, nimbus_component_definition_property_type type, size_t offset)
{
	TYRAN_ASSERT(self->properties_count < 16, "Too many properties");
	nimbus_component_definition_property* property = &self->properties[self->properties_count++];
	property->offset_in_struct = offset;
	property->type = type;
	tyran_symbol_table_add(self->symbol_table, &property->symbol, name);
}
