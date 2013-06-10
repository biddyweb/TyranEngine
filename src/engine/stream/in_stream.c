#include "in_stream.h"
#include "endian.h"
#include <tyranscript/tyran_clib.h>

in_stream* nimbus_in_stream_new(tyran_memory* memory, int max_octets)
{
    nimbus_in_stream* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_in_stream);
    self->buffer = TYRAN_MEMORY_ALLOC(memory, max_octets, "in_stream buffer");
    self->pointer = buffer;
    self->end_buffer = buffer + max_octets;
}

void nimbus_in_stream_free(in_stream* self)
{
    TYRAN_FREE(self->buffer);
    TYRAN_FREE(self);
}

int nimbus_in_stream_read_octets(in_stream* self, void* data, int length)
{
    TYRAN_ASSERT(self->pointer + length < self->end_buffer, "Read too far");
    tyran_memcpy_octets(data, self->pointer, length);
    self->pointer += length;
    
    return 0;
}

int nimbus_in_stream_read_u16t(in_stream* self, u16t* data)
{
    nimbus_in_stream_read_octets(self, data, 2);
    *data = nimbus_endian_u16t_from_network(*data);
    
    return 0;
}

int nimbus_in_stream_read_u32t(in_stream* self, u32t* data)
{
    nimbus_in_stream_read_octets(self, data, 4);
    *data = nimbus_endian_u32t_from_network(*data);
    
    return 0;
}

int nimbus_in_stream_read_string(in_stream* self, char* data, int max_length)
{
    u16t len;
    nimbus_in_stream_read_u16t(self, &len);
    TYRAN_ASSERT(len < max_length, "String is too big for buffer");
    
    nimbus_in_stream_read_octets(self, data, len);
    data[len] = 0;
    
    return 0;
}
