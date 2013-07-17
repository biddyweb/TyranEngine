#ifndef nimbus_event_module_resource_updated_h
#define nimbus_event_module_resource_updated_h

#include <tyran_engine/resource/id.h>

typedef int module_resource_index;
struct nimbus_event_write_stream;

typedef struct nimbus_event_module_resource_updated {
	module_resource_index track_index;
} nimbus_event_module_resource_updated;

void nimbus_event_module_resource_updated_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id, module_resource_index index);

#endif
