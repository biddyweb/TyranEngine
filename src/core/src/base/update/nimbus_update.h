#ifndef nimbus_update_h
#define nimbus_update_h

#include <tyranscript/tyran_types.h>

#include "../event/nimbus_event_stream.h"

typedef struct nimbus_update {
    nimbus_event_write_stream event_write_stream;
    nimbus_event_read_stream event_read_stream;
} nimbus_update;


void nimbus_update_init(nimbus_update* self, struct tyran_memory* memory);

#endif
