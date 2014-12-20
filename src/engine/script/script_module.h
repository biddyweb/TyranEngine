#ifndef nimbus_script_module_h
#define nimbus_script_module_h

#include <tyran_engine/resource/type_id.h>
#include <tyran_core/update/update.h>

#include "../state/state.h"
#include <tyran_engine/state/event_component_header.h>
#include <mocha/runtime.h>
#include <mocha/values.h>

struct nimbus_modules;
struct tyran_memory;

extern const u8t NIMBUS_EVENT_SCRIPT_UPDATED_ID;

typedef struct nimbus_script_updated {
    nimbus_event_component_header component;
    void* script;
} nimbus_script_updated;


typedef struct nimbus_script_module {
	nimbus_update update;
	nimbus_resource_type_id script_type_id;
	nimbus_resource_type_id state_type_id;
	u8t* script_buffer;
	int script_buffer_size;
	struct nimbus_modules* modules;
	struct tyran_memory* memory;
	nimbus_state main_state;
	mocha_runtime runtime;
	mocha_context root_context;
	mocha_values values;
	const struct mocha_object* type_keyword;
} nimbus_script_module;

void nimbus_script_module_init(void* _self, struct tyran_memory* memory);

#endif
