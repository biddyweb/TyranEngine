#include "ring_buffer.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>

void nimbus_ring_buffer_init(nimbus_ring_buffer* self, tyran_memory* memory, int max_length)
{
	self->buffer = TYRAN_MEMORY_ALLOC(memory, max_length, "circular buffer");
	self->max_size = max_length;
	self->size = 0;
	self->read_index = 0;
	self->write_index = 0;
}

void nimbus_ring_buffer_write_pointer(nimbus_ring_buffer* self, u8t** data, int* length)
{
	*data = self->buffer + self->write_index;
	if (self->size == self->max_size) {
		*data = 0;
		*length = 0;
	} else if (self->write_index >= self->read_index) {
		*length = self->max_size - self->write_index;
	} else {
		*length = self->read_index - self->write_index;
	}
}

int nimbus_ring_buffer_size(nimbus_ring_buffer* self)
{
	return self->size;
}

void nimbus_ring_buffer_free(nimbus_ring_buffer* self)
{
	TYRAN_MEMORY_FREE(self->buffer);
}

void nimbus_ring_buffer_write_pointer_advance(nimbus_ring_buffer* self, int size)
{
	self->write_index += size;
	self->write_index %= self->max_size;
	self->size += size;
}

/*
static void write_advance(nimbus_ring_buffer* self, const u8t* data_pointer, int size)
{
	tyran_memcpy_octets(self->buffer + self->write_index, data_pointer, size);
	nimbus_ring_buffer_write_pointer_advance(self, size);
}
*/

void nimbus_ring_buffer_read_pointer_advance(nimbus_ring_buffer* self, int size)
{
	self->read_index += size;
	self->read_index %= self->max_size;
	self->size -= size;
}

static void read_advance(nimbus_ring_buffer* self, u8t* data_pointer, int size)
{
	tyran_memcpy_octets(data_pointer, self->buffer + self->read_index, size);
	nimbus_ring_buffer_read_pointer_advance(self, size);
}

void nimbus_ring_buffer_read_pointer(nimbus_ring_buffer* self, u8t** data, int* length)
{
	int available;
	if (self->size == 0) {
		available = 0;
	} else if (self->read_index < self->write_index) {
		available = self->write_index - self->read_index;
	} else {
		available = self->max_size - self->read_index;
	}

	TYRAN_LOG("read ri:%d, wi:%d, len:%d", self->read_index, self->write_index, available);

	*data = self->buffer + self->read_index;
	*length = available;
}


void nimbus_ring_buffer_read(nimbus_ring_buffer* self, u8t* data, int size)
{
	u8t* source;
	int available;

	TYRAN_ASSERT(size <= self->size, "Tried to read more than buffer size");

	nimbus_ring_buffer_read_pointer(self, &source, &available);
	int read_count = available > size ? size : available;
	read_advance(self, data, read_count);
	size -= read_count;

	nimbus_ring_buffer_read_pointer(self, &source,  &available);
	int next_count = available > size ? size : available;
	read_advance(self, data + read_count, next_count);
	size -= next_count;

	TYRAN_ASSERT(size == 0, "Buffer didn't hold it");
}
