#include "nimbus_modules.h"

void nimbus_modules_init(nimbus_modules* self)
{
	self->modules_count = 0;
	self->modules_max_count = 16;
}

void nimbus_modules_add(nimbus_modules* self, const char* name, nimbus_module_create_function func)
{
	nimbus_module* module = &self->modules[self->modules_count++];
	module->func = func;
	module->name = name;
}
