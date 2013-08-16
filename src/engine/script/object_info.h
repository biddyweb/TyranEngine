#ifndef nimbus_object_info_h
#define nimbus_object_info_h

#include <tyranscript/tyran_symbol.h>

struct nimbus_event_definition;

typedef struct nimbus_object_info {
	int instance_id;
	int instance_index;
	const struct nimbus_event_definition* event_definition;
	tyran_boolean is_module_resource;
	tyran_boolean is_spawned_combine;
} nimbus_object_info;

#endif
