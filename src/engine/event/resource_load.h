#ifndef nimbus_resource_load_h
#define nimbus_resource_load_h

#include "../resource/resource_id.h"

const u8t NIMBUS_EVENT_RESOURCE_LOAD = 1;

typedef struct nimbus_resource_load {
	nimbus_resource_id resource_id;
} nimbus_resource_load;

#endif