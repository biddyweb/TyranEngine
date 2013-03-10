#ifndef nimbus_event_stream_h
#define nimbus_event_stream_h

#include <tyranscript/tyran_types.h>

typedef u8t nimbus_event_type_id;

typedef struct nimbus_event_stream {
	const u8t* pointer;
	nimbus_event_type_id event_type_id;
} nimbus_event_stream;

#endif
