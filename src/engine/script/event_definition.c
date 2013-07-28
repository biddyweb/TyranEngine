#include "event_definition.h"
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

void nimbus_event_definition_add_property(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type)
{
	nimbus_event_definition_property* property = &self->properties[self->properties_count++];
	tyran_symbol_table_add(self->symbol_table, &property->symbol, name);
	property->type = type;
}
