#ifndef nimbus_object_loader_h
#define nimbus_object_loader_h

#include <tyran_core/update/update.h>
#include <tyran_engine/resource/type_id.h>
#include "dependency_resolver.h"

struct tyran_mocha_api;
struct tyran_value;

typedef struct nimbus_object_loader {
	struct tyran_mocha_api* mocha;
	struct tyran_value* context;
	nimbus_update update;
	u8t* script_buffer;
	int script_buffer_size;
	nimbus_dependency_resolver dependency_resolver;
	nimbus_resource_type_id state_type_id;
	nimbus_resource_type_id object_type_id;
	nimbus_resource_type_id wire_object_type_id;
	nimbus_resource_type_id script_object_type_id;
	nimbus_resource_id waiting_for_state_resource_id;
} nimbus_object_loader;

void nimbus_object_loader_init(nimbus_object_loader* self, struct tyran_memory* memory, struct tyran_mocha_api* mocha, struct tyran_value* context);
void nimbus_object_loader_free(nimbus_object_loader* self);
#endif
