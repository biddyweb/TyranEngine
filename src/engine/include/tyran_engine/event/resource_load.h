#ifndef nimbus_resource_load_h
#define nimbus_resource_load_h

#include <tyran_engine/resource/id.h>

extern const u8t NIMBUS_EVENT_RESOURCE_LOAD;

struct nimbus_event_write_stream;

typedef struct nimbus_resource_load {
	nimbus_resource_id resource_id;
} nimbus_resource_load;

void nimbus_resource_load_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id);


#endif
