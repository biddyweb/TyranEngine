#include <tyran_engine/module/modules.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

#include <tyran_engine/state/component_definition.h>
#include <tyran_engine/resource/resource_definition.h>

void nimbus_modules_init(nimbus_modules* self, struct tyran_memory* memory, struct tyran_symbol_table* symbol_table)
{
	self->modules_count = 0;
	self->modules_max_count = 16;
	self->event_definitions_max_count = 64;
	self->event_definitions_count = 0;
	self->symbol_table = symbol_table;
	self->event_definitions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_event_definition, self->event_definitions_max_count);
	
	self->component_definitions_max_count = 64;
	self->component_definitions_count = 0;
	self->component_definitions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_component_definition, self->component_definitions_max_count);

	nimbus_resource_definitions_init(&self->resource_definitions, memory);
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

const nimbus_component_definition* nimbus_modules_component_definition_from_type(nimbus_modules* self, tyran_symbol type)
{
	for (int i = 0; i < self->component_definitions_count; ++i) {
		const nimbus_component_definition* definition = &self->component_definitions[ i ];
		if (tyran_symbol_equal(&definition->type_symbol, &type)) {
			return definition;
		}
	}
	const char* type_string = tyran_symbol_table_lookup(self->symbol_table, &type);
	
	TYRAN_ERROR("Could not find definition from type:%d '%s'", type.hash, type_string);

	return 0;
}

nimbus_event_definition* nimbus_modules_add_event_struct(nimbus_modules* self, const char* name, size_t struct_size, u8t event_type_id)
{
	nimbus_event_definition* definition = &self->event_definitions[self->event_definitions_count++];
	nimbus_event_definition_init(definition, self->symbol_table, "x", event_type_id, 0);
	tyran_symbol_table_add(self->symbol_table, &definition->struct_symbol, name);
	definition->struct_size = struct_size;

	return definition;
}

void nimbus_modules_add_resource_definition(nimbus_modules* self, const char* name, size_t struct_size)
{
	nimbus_resource_type_id type_id = nimbus_resource_type_id_from_string(name);
	nimbus_resource_definitions_create(&self->resource_definitions, type_id, struct_size, name);
}
