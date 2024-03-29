#include "out_stream.h"
#include "endian.h"
#include <tyranscript/tyran_clib.h>
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

void nimbus_out_stream_clear(nimbus_out_stream* self)
{
	self->pointer = self->buffer;
}

void nimbus_out_stream_init(nimbus_out_stream* self, tyran_memory* memory, int max_octets)
{
	self->buffer = TYRAN_MEMORY_ALLOC(memory, max_octets, "out_stream buffer");
	self->end_buffer = self->buffer + max_octets;
	nimbus_out_stream_clear(self);
}

void nimbus_out_stream_free(nimbus_out_stream* self)
{
	TYRAN_MEMORY_FREE(self->buffer);
	self->end_buffer = 0;
}

void nimbus_out_stream_write_octets(nimbus_out_stream* self, const void* data, int octet_length)
{
	TYRAN_ASSERT(self->pointer + octet_length <= self->end_buffer, "Overwrite buffer");
	tyran_memcpy_octets(self->pointer, data, octet_length);
	self->pointer += octet_length;
}

void nimbus_out_stream_write_u8(nimbus_out_stream* self, u8t data)
{
	nimbus_out_stream_write_octets(self, &data, 1);
}

#if !defined TORNADO_OS_NACL

void nimbus_out_stream_write_u16(nimbus_out_stream* self, u16t data)
{
	u16t network_short = nimbus_endian_u16_to_network(data);
	nimbus_out_stream_write_octets(self, &network_short, 2);
}

void nimbus_out_stream_write_u32(nimbus_out_stream* self, u32t data)
{
	u32t network_long = nimbus_endian_u32_to_network(data);
	nimbus_out_stream_write_octets(self, &network_long, 4);
}

void nimbus_out_stream_write_string(nimbus_out_stream* self, const char* str)
{
	u16t len = tyran_strlen(str);
	nimbus_out_stream_write_u16(self, len);
	nimbus_out_stream_write_octets(self, str, len);
}
#endif

void nimbus_out_stream_info(nimbus_out_stream* self, const u8t** buffer, int* length)
{
	*buffer = self->buffer;
	*length = (int) (self->pointer - self->buffer);
}
