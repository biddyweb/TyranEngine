#include "nimbus_event_stream.h"
#include <tyranscript/tyran_clib.h>

void nimbus_event_write_stream_init(nimbus_event_write_stream* stream, u8t* data, int max_length)
{
	stream->pointer = data;
	stream->end_pointer = data + max_length;
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
