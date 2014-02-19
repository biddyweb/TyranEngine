#ifndef nimbus_script_module_h
#define nimbus_script_module_h

#include <tyranscript/tyran_mocha_api.h>
#include <tyran_engine/resource/type_id.h>

typedef struct nimbus_script_module {
	nimbus_update update;
	tyran_mocha_api mocha_api;
	nimbus_resource_type_id script_type_id;
	nimbus_resource_type_id combine_script_type_id;
	u8t* script_buffer;
	int script_buffer_size;
	struct tyran_object* context;
} nimbus_script_module;

#endif
