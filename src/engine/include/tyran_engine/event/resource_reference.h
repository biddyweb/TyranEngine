#ifndef nimbus_event_resource_reference_h
#define nimbus_event_resource_reference_h

#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>

typedef struct nimbus_event_component_header {
	tyran_boolean is_used;
} nimbus_event_component_header;

typedef struct nimbus_event_resource_reference {
	nimbus_resource_id resource_id;
	nimbus_resource_type_id resource_type_id;
} nimbus_event_resource_reference;

#endif
