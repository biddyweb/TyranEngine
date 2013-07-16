#include <tyran_engine/module/modules.h>

void nimbus_modules_init(nimbus_modules* self, struct tyran_memory* memory)
{
	self->modules_count = 0;
	self->modules_max_count = 16;
	self->event_definitions_max_count = 64;
	self->event_definitions_count = 0;
	self->event_definitions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_event_definition, self->event_definitions_max_count);
}

void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset)
{
	nimbus_module* module = &self->modules[self->modules_count++];
	module->init_func = func;
	module->name = name;
	module->octet_size = octet_size;
	module->update_offset = update_offset;
}

nimbus_event_definition* nimbus_modules_add_event(nimbus_modules* self, const char* name, u8t event_type_id)
{
	nimbus_event_definition* definition = &self->event_definitions[self->event_definitions_count++];
	return definition;
}