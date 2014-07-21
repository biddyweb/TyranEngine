#ifndef nimbus_extra_reference_h
#define nimbus_extra_reference_h

#include <tyran_engine/resource/id.h>

typedef struct nimbus_extra_reference {
	nimbus_resource_id resource_id;
	void* pointer;
	tyran_symbol component_name;
	tyran_symbol property_name;
} nimbus_extra_reference;

#endif
