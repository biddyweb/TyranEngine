#include "out_stream.h"
#include "endian.h"
#include <tyranscript/tyran_clib.h>

void nimbus_out_stream_new(tyran_memory* memory, int max_octets)
{
    nimbus_out_stream* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_out_stream);
    self->buffer = TYRAN_MEMORY_ALLOC(memory, max_octets, "out_stream buffer");
    self->pointer = self->buffer;
    self->end_buffer = self->buffer + max_octets;
}

void nimbus_out_stream_free(nimbus_out_stream* self)
{
    TYRAN_MEMORY_FREE(self->buffer);
    TYRAN_MEMORY_FREE(self);
    self->end_buffer = 0;
}

void nimbus_out_stream_write_octets(nimbus_out_stream* self, const void* data, int octet_length)
{
    TYRAN_ASSERT(self->pointer + octet_length < self->end_of_buffer, "Overwrite buffer");
    tyran_memcpy_octets(self->pointer, data, octet_length);
    self->pointer += octet_length;
}

void nimbus_out_stream_write_u16t(nimbus_out_stream* self, u16t data)
{
    u16t network_short = nimbus_endian_u16t_to_network(data);
    nimbus_out_stream_write_octets(self, &network_short, 2);
}

void nimbus_out_stream_write_u32t(nimbus_out_stream* self, u32t data)
{
    u32t network_long = nimbus_endian_u16t_to_network(data);
    nimbus_out_stream_write_octets(self, &network_long, 4);
}

void nimbus_out_stream_write_string(nimbus_out_stream* self, const char* str)
{
    u16t len = tyran_strlen(str);
    nimbus_out_stream_write_u16t(self, len);
    nimbus_out_stream_write_octets(self, str, len);
}

void nimbus_out_stream_info(nimbus_out_stream* self, u8t** buffer, int* length)
{
    *buffer = self->buffer;
    *length = self->pointer - self->buffer;
}
