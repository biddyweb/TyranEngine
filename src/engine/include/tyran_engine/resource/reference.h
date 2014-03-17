#ifndef nimbus_resource_reference_h
#define nimbus_resource_reference_h

#include "id.h"
#include "type_id.h"

typedef struct nimbus_resource_reference {
	nimbus_resource_type_id type_id;
	nimbus_resource_id id;
} nimbus_resource_reference;

#endif
