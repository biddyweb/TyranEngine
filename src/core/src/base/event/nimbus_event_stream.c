#include "nimbus_event_stream.h"
#include <tyranscript/tyran_clib.h>


void nimbus_event_write_stream_clear(nimbus_event_write_stream* self)
{
	self->pointer = self->buffer;
}

void nimbus_event_write_stream_init(nimbus_event_write_stream* self, tyran_memory* memory, int max_length)
{
	self->buffer = TYRAN_MEMORY_ALLOC(memory, max_length, "event write buffer");
	nimbus_event_write_stream_clear(self);
	self->end_pointer = self->buffer + max_length;
}

void nimbus_event_write_stream_free(nimbus_event_write_stream* stream)
{
	TYRAN_MEMORY_FREE(stream->buffer);
}

int nimbus_event_write_stream_length(nimbus_event_write_stream* self)
{
    return self->end_pointer - self->buffer;
}


void nimbus_event_stream_write_octets(nimbus_event_write_stream* stream, const void* data, int len)
{
	TYRAN_ASSERT(stream->pointer + len < stream->end_pointer, "Overwrite!");
	tyran_memcpy_type(u8t, stream->pointer, data, len);
	stream->pointer += len;
}

void nimbus_event_stream_read_octets(nimbus_event_read_stream* stream, u8t* data, int len)
{
	tyran_memcpy_type(u8t, data, stream->pointer, len);
	stream->pointer += len;
}

void nimbus_event_stream_read_init(nimbus_event_read_stream* self, const u8t* pointer, int length)
{
    self->pointer = pointer;
    self->end_pointer = pointer + length;
}
