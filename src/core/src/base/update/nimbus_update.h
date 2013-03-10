#ifndef nimbus_update_h
#define nimbus_update_h

#include <tyranscript/tyran_types.h>

typedef struct nimbus_update {
	u8t* outgoing_events;
	int outgoing_event_count;

	u8t* event_buffer;
	int event_buffer_size;
} nimbus_update;

#endif
