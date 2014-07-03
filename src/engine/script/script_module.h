#ifndef nimbus_script_module_h
#define nimbus_script_module_h

#include <tyran_engine/resource/type_id.h>
#include <tyran_core/update/update.h>

#include "../state/state.h"

struct nimbus_modules;
struct tyran_memory;

typedef struct nimbus_script_module {
	nimbus_update update;
	nimbus_resource_type_id script_type_id;
	nimbus_resource_type_id combine_script_type_id;
	u8t* script_buffer;
	int script_buffer_size;
	struct nimbus_modules* modules;
	struct tyran_memory* memory;
	nimbus_state main_state;
} nimbus_script_module;

void nimbus_script_module_init(void* _self, struct tyran_memory* memory);

#endif
