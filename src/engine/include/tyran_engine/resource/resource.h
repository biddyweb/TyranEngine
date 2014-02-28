#ifndef nimbus_resource_h
#define nimbus_resource_h

#include "id.h"

typedef struct nimbus_resource {
	nimbus_resource_id id;
	tyran_boolean is_loaded;
} nimbus_resource;

#endif
