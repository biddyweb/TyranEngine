#ifndef nimbus_resource_h
#define nimbus_resource_h

#include "id.h"
#include "type_id.h"
#include "index.h"

typedef struct nimbus_resource {
	nimbus_resource_id id;
    nimbus_resource_index index;
	nimbus_resource_type_id type_id;
	tyran_boolean is_loaded;
} nimbus_resource;

#endif
