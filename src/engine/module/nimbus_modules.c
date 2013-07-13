#include <tyran_engine/module/modules.h>

void nimbus_modules_init(nimbus_modules* self)
{
	self->modules_count = 0;
	self->modules_max_count = 16;
}

void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset)
{
	nimbus_module* module = &self->modules[self->modules_count++];
	module->init_func = func;
	module->name = name;
	module->octet_size = octet_size;
	module->update_offset = update_offset;
}
