#ifndef nimbus_resource_load_state_h
#define nimbus_resource_load_state_h

#include <tyran_engine/resource/id.h>

extern const u8t NIMBUS_EVENT_RESOURCE_LOAD_STATE;

struct nimbus_event_write_stream;

typedef struct nimbus_resource_load_state {
	nimbus_resource_id resource_id;
} nimbus_resource_load_state;

void nimbus_resource_load_state_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id);


#endif
