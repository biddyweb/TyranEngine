#ifndef nimbus_script_object_updated_h
#define nimbus_script_object_updated_h

#include <tyran_engine/resource/id.h>

struct tyran_object;
struct nimbus_event_write_stream;

extern const u8t NIMBUS_EVENT_SCRIPT_OBJECT_UPDATED_ID;

typedef struct nimbus_script_object_updated {
	nimbus_resource_id resource_id;
	struct tyran_object* object;
} nimbus_script_object_updated;

void nimbus_script_object_updated_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id,
									   struct tyran_object* object);

#endif
