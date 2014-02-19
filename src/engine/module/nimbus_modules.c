#include <tyran_engine/module/modules.h>
#include <tyranscript/tyran_symbol_table.h>

#include <tyran_engine/state/component_definition.h>

void nimbus_modules_init(nimbus_modules* self, struct tyran_memory* memory, struct tyran_symbol_table* symbol_table)
{
	self->modules_count = 0;
	self->modules_max_count = 16;
	self->event_definitions_max_count = 64;
	self->event_definitions_count = 0;
	self->symbol_table = symbol_table;
	self->event_definitions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_event_definition, self->event_definitions_max_count);
}

static nimbus_module* add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset)
{
	nimbus_module* module = &self->modules[self->modules_count++];
	module->init_func = func;
	module->name = name;
	module->octet_size = octet_size;
	module->update_offset = update_offset;
	module->affinity = -1;
	return module;
}

void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset)
{
	add(self, name, octet_size, func, update_offset);
}

void nimbus_modules_add_affinity(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset, int affinity)
{
	nimbus_module* module = add(self, name, octet_size, func, update_offset);
	module->affinity = affinity;
}

nimbus_event_definition* nimbus_modules_add_event(nimbus_modules* self, const char* name, u8t event_type_id, u8t unspawn_event_type_id)
{
	nimbus_event_definition* definition = &self->event_definitions[self->event_definitions_count++];
	nimbus_event_definition_init(definition, self->symbol_table, name, event_type_id, unspawn_event_type_id);

	return definition;
}

nimbus_component_definition* nimbus_modules_add_component_definition(nimbus_modules* self, const char* name, u8t event_type_id, size_t struct_size)
{
	nimbus_component_definition* definition = &self->component_definitions[self->component_definitions_count++];
	nimbus_component_definition_init(definition, self->symbol_table, name, event_type_id, struct_size);
	
	return definition;
}


nimbus_event_definition* nimbus_modules_add_event_struct(nimbus_modules* self, const char* name, size_t struct_size, u8t event_type_id)
{
	nimbus_event_definition* definition = &self->event_definitions[self->event_definitions_count++];
	nimbus_event_definition_init(definition, self->symbol_table, "x", event_type_id, 0);
	tyran_symbol_table_add(self->symbol_table, &definition->struct_symbol, name);
	definition->struct_size = struct_size;

	return definition;
}
