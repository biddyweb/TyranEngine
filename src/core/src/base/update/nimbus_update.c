#include "nimbus_update.h"
#include <tyranscript/tyran_log.h>

void nimbus_update_update(void* _self, struct nimbus_task_queue* queue)
{
	nimbus_update* self = _self;
	TYRAN_LOG("internal UPDATE, %p name:'%s'", _self, self->name);
	
	nimbus_event_read_stream* read_stream = &self->event_read_stream;
	int read_stream_length = read_stream->end_pointer - read_stream->pointer;
	TYRAN_LOG("Processing %d octets in read stream", read_stream_length);
	nimbus_event_process(&self->event_listener, read_stream->pointer, read_stream_length);
	
	TYRAN_ASSERT(self->update_function != 0, "OMG POITNER BAD!");
	self->update_function(self->update_function_self);
	TYRAN_LOG("internal update function done");
}

void nimbus_update_init(nimbus_update* self, struct tyran_memory* memory, nimbus_update_function func, void* update_self)
{
    const int max_size = 1024;
    TYRAN_LOG("Valid update pointer:%p", (void*)self);
    TYRAN_ASSERT(func != 0, "Must give me a proper function");
    self->update_function = func;
    self->update_function_self = update_self;
    nimbus_event_write_stream_init(&self->event_write_stream, memory, max_size);
    self->name = "TJOHO NAME!";
    nimbus_task_init(&self->task, nimbus_update_update, self);
}

