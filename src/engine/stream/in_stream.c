#include "in_stream.h"
#include "endian.h"
#include <tyranscript/tyran_clib.h>

nimbus_in_stream* nimbus_in_stream_new(tyran_memory* memory, const u8t* buffer, int max_octets)
{
	nimbus_in_stream* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_in_stream);
	self->buffer = buffer;
	self->pointer = self->buffer;
	self->end_buffer = self->buffer + max_octets;

	return self;
}

void nimbus_in_stream_free(nimbus_in_stream* self)
{
	TYRAN_MEMORY_FREE(self);
}

int nimbus_in_stream_read_octets(nimbus_in_stream* self, void* data, int length)
{
	TYRAN_ASSERT(self->pointer + length < self->end_buffer, "Read too far");
	tyran_memcpy_octets(data, self->pointer, length);
	self->pointer += length;

	return 0;
}

int nimbus_in_stream_read_u8(nimbus_in_stream* self, u8t* data)
{
	nimbus_in_stream_read_octets(self, data, 1);
}

int nimbus_in_stream_read_u16(nimbus_in_stream* self, u16t* data)
{
	nimbus_in_stream_read_octets(self, data, 2);
	*data = nimbus_endian_u16_from_network(*data);

	return 0;
}

int nimbus_in_stream_read_u32(nimbus_in_stream* self, u32t* data)
{
	nimbus_in_stream_read_octets(self, data, 4);
	*data = nimbus_endian_u32_from_network(*data);

	return 0;
}

int nimbus_in_stream_read_string(nimbus_in_stream* self, char* data, int max_length)
{
	u16t len;
	nimbus_in_stream_read_u16(self, &len);
	TYRAN_ASSERT(len < max_length, "String is too big for buffer");

	nimbus_in_stream_read_octets(self, data, len);
	data[len] = 0;

	return 0;
}
