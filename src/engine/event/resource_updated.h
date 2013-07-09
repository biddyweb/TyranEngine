#ifndef nimbus_resource_updated_h
#define nimbus_resource_updated_h

#include "../resource/resource_id.h"
#include "../resource/resource_type_id.h"

struct nimbus_event_write_stream;

extern const u8t NIMBUS_EVENT_RESOURCE_UPDATED;

typedef struct nimbus_resource_updated {
	nimbus_resource_id resource_id;
	nimbus_resource_type_id resource_type_id;
	int payload_size;
} nimbus_resource_updated;


void nimbus_resource_updated_send(struct nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, void* data, int payload_size);

#endif
