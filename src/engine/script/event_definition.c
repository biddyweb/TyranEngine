#include <tyran_engine/event_definition/event_definition.h>
#include <tyranscript/tyran_symbol_table.h>

void nimbus_event_definition_init(nimbus_event_definition* self, struct tyran_symbol_table* symbol_table, const char* name, u8t event_type_id, u8t unspawn_event_type_id)
{
	self->symbol_table = symbol_table;
	self->properties_count = 0;
	self->name = name;
	self->event_type_id = event_type_id;
	self->unspawn_event_type_id = unspawn_event_type_id;
	self->has_index = TYRAN_TRUE;
	self->is_module_to_script = TYRAN_FALSE;
	tyran_symbol_table_add(self->symbol_table, &self->type_symbol, name);
}

static nimbus_event_definition_property* add_property(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type)
{
	nimbus_event_definition_property* property = &self->properties[self->properties_count++];
	tyran_symbol_table_add(self->symbol_table, &property->symbol, name);
	property->type = type;

	return property;
}

void nimbus_event_definition_add_property(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type)
{
	add_property(self, name, type);
}

nimbus_event_definition_property* nimbus_event_definition_add_property_ex(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type)
{
	return add_property(self, name, type);
}

void nimbus_event_definition_add_property_array(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type)
{
	nimbus_event_definition_property* property = add_property(self, name, type);
	property->is_array = TYRAN_TRUE;
}

void nimbus_event_definition_add_property_struct(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type, size_t offset)
{
	nimbus_event_definition_property* property = add_property(self, name, type);
	property->offset_in_struct = offset;
}
