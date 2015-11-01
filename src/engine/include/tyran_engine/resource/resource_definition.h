#ifndef nimbus_resource_definition_h
#define nimbus_resource_definition_h

#include <tyran_engine/resource/type_id.h>

typedef struct nimbus_resource_definition {
	const char* debug_string;
	size_t struct_size;
	nimbus_resource_type_id resource_type_id;
} nimbus_resource_definition;

#endif
