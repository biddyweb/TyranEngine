#include <tyran_engine/state/component_definition.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_log.h>
void nimbus_component_definition_init(nimbus_component_definition* self, struct tyran_symbol_table* symbol_table, const char* debug_name, u8t event_type_id, size_t struct_size)
{
	self->symbol_table = symbol_table;
	self->debug_name = debug_name;
	self->event_type_id = event_type_id;
	self->properties_count = 0;
	self->struct_size = struct_size;
	tyran_symbol_table_add(self->symbol_table, &self->type_symbol, debug_name);
}

static nimbus_component_definition_property* add_property(nimbus_component_definition* self, const char* name, nimbus_component_definition_property_type type, size_t offset)
{
	TYRAN_ASSERT(self->properties_count < 16, "Too many properties");
	nimbus_component_definition_property* property = &self->properties[self->properties_count++];
	property->offset_in_struct = offset;
	property->type = type;
	tyran_symbol_table_add(self->symbol_table, &property->symbol, name);
	return property;
}


void nimbus_component_definition_add_property(nimbus_component_definition* self, const char* name, nimbus_component_definition_property_type type, size_t offset)
{
	add_property(self, name, type, offset);
}

void nimbus_component_definition_add_property_object(nimbus_component_definition* self, const char* name, nimbus_component_definition* object_definition, size_t offset)
{
	nimbus_component_definition_property* property = add_property(self, name, NIMBUS_COMPONENT_DEFINITION_OBJECT, offset);
	property->object_definition = object_definition;
}

const nimbus_component_definition_property* nimbus_component_definition_property_from_name(const nimbus_component_definition* self, tyran_symbol symbol)
{
	for (int i=0; i<self->properties_count; ++i) {
		const nimbus_component_definition_property* property = &self->properties[i];
		if (tyran_symbol_equal(&property->symbol, &symbol)) {
			return property;
		}
	}
	TYRAN_ERROR("Couldn't find property from symbol:%d", symbol.hash);
	return 0;
}
