#ifndef nimbus_modules_h
#define nimbus_modules_h

#include <tyran_core/update/update.h>
#include "nimbus_module.h"


typedef struct nimbus_modules {
	nimbus_module modules[16];
	int modules_count;
	int modules_max_count;
} nimbus_modules;

void nimbus_modules_init(nimbus_modules* self);
void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset);

#endif
