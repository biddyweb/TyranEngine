#ifndef nimbus_modules_h
#define nimbus_modules_h

#include <tyran_core/update/update.h>

typedef nimbus_update* (*nimbus_module_create_function)(struct tyran_memory* memory);

typedef struct nimbus_module {
	const char* name;
	nimbus_module_create_function func;
} nimbus_module;


typedef struct nimbus_modules {
	nimbus_module modules[16];
	int modules_count;
	int modules_max_count;
} nimbus_modules;

void nimbus_modules_init(nimbus_modules* self);
void nimbus_modules_add(nimbus_modules* self, const char* name, nimbus_module_create_function func);

#endif
