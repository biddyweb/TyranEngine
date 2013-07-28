#ifndef nimbus_event_unspawn_h
#define nimbus_event_unspawn_h

#include <tyran_engine/resource/id.h>

struct nimbus_event_write_stream;

typedef struct nimbus_event_unspawn {
	int instance_index;
} nimbus_event_unspawn;

void nimbus_event_unspawn_send(struct nimbus_event_write_stream* stream, u8t event_type_id, int instance_index);

#endif
