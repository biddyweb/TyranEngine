#ifndef nimbus_module_h
#define nimbus_module_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;
struct nimbus_update;

typedef void (*nimbus_module_init_function)(void* _self, struct tyran_memory* memory);

typedef struct nimbus_module {
	const char* name;
	nimbus_module_init_function init_func;
	size_t update_offset;
	size_t octet_size;
} nimbus_module;

void* nimbus_module_create(struct nimbus_module* self, struct tyran_memory* memory);
struct nimbus_update* nimbus_module_get_update(nimbus_module* self, void* struct_memory);

#endif
