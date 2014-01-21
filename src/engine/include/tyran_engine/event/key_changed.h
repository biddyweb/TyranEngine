#ifndef nimbus_key_changed_h
#define nimbus_key_changed_h

#include <tyran_core/event/event_stream.h>

extern const nimbus_event_type_id NIMBUS_EVENT_KEY_CHANGED_ID;

struct nimbus_event_write_stream;

typedef struct nimbus_key_changed {
	int virtual_key_id;
	tyran_boolean key_is_down;
} nimbus_key_changed;

void nimbus_key_changed_send(struct nimbus_event_write_stream* stream, u32t virtual_key_id, tyran_boolean key_is_down);

#endif
