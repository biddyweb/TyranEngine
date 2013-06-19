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
    return self->pointer - self->buffer;
}


void nimbus_event_stream_write_octets(nimbus_event_write_stream* stream, const void* data, int len)
{
	TYRAN_ASSERT(stream->pointer + len < stream->end_pointer, "Overwrite!");
	TYRAN_LOG("Writing event stream %d", len);
	tyran_memcpy_type(u8t, stream->pointer, data, len);
	stream->pointer += len;
}

void nimbus_event_stream_write_align(nimbus_event_write_stream* self)
{
	const int alignment = 8;
	if (((tyran_pointer_to_number)self->pointer % alignment) != 0) {
		self->pointer += alignment - ((tyran_pointer_to_number)self->pointer % alignment);
	}
}

void nimbus_event_stream_read_octets(nimbus_event_read_stream* stream, u8t* data, int len)
{
	tyran_memcpy_type(u8t, data, stream->pointer, len);
	stream->pointer += len;
}

void nimbus_event_stream_read_init(nimbus_event_read_stream* self, const u8t* pointer, int length)
{
	TYRAN_LOG("Setting read to %p with %d octets", (const void*) pointer, length);
    self->pointer = pointer;
    self->end_pointer = pointer + length;
}

void nimbus_event_stream_read_skip(nimbus_event_read_stream* self, int length)
{
	self->pointer += length;
}

void nimbus_event_stream_read_align(nimbus_event_read_stream* self)
{
	const int alignment = 8;
	if (((tyran_pointer_to_number)self->pointer % alignment) != 0) {
		nimbus_event_stream_read_skip(self, alignment - ((tyran_pointer_to_number)self->pointer % alignment) );
	}
}
