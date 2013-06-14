#include "nimbus_update.h"

void nimbus_update_init(nimbus_update* self, struct tyran_memory* memory)
{
    const int max_size = 1024;
    nimbus_event_write_stream_init(&self->event_write_stream, memory, max_size);
}
