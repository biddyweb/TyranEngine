#include "game.h"
#include "../module/nimbus_modules.h"

static nimbus_update* create_renderer(struct tyran_memory* memory)
{
	return 0;
}

void nimbus_register_modules(nimbus_modules* modules)
{
	nimbus_modules_add(modules, "renderer", create_renderer);
}
