#include "ring_buffer.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>

void nimbus_ring_buffer_init(nimbus_ring_buffer* self, tyran_memory* memory, int max_length)
{
	self->buffer = TYRAN_MEMORY_ALLOC(memory, max_length, "circular buffer");
	self->max_size = max_length;
}

void nimbus_ring_buffer_write_pointer(nimbus_ring_buffer* self, u8t** data, int* length)
{
	*data = self->buffer + self->write_index;
	*length = self->max_size - self->write_index;
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

static void write_advance(nimbus_ring_buffer* self, const u8t* data_pointer, int size)
{
	tyran_memcpy_octets(self->buffer + self->write_index, data_pointer, size);
	nimbus_ring_buffer_write_pointer_advance(self, size);
}

static void read_advance_pointer(nimbus_ring_buffer* self, int size)
{
	self->read_index += size;
	self->read_index %= self->max_size;
	self->size -= size;
}

static void read_advance(nimbus_ring_buffer* self, u8t* data_pointer, int size)
{
	tyran_memcpy_octets(data_pointer, self->buffer + self->read_index, size);
	read_advance_pointer(self, size);
}

void nimbus_ring_buffer_read_pointer(nimbus_ring_buffer* self, int size, u8t** data, int* length)
{
	int available;
	if (self->read_index < self->write_index) {
		available = self->write_index - self->read_index;
	} else {
		available = self->max_size - self->read_index;
	}

	*data = self->buffer + self->read_index;
	*length = available;
	read_advance_pointer(self, available);
}


int nimbus_ring_buffer_write(nimbus_ring_buffer* self, const void* data, int len)
{
	int write_octets_left = self->max_size - self->size;
	if (len > write_octets_left) {
		return -1;
	}

	const u8t* data_pointer = (const u8t*) data;

	int size_to_end_of_buffer = self->max_size - self->write_index;
	if (len > size_to_end_of_buffer) {
		write_advance(self, data_pointer, size_to_end_of_buffer);
		data_pointer += size_to_end_of_buffer;
		len -= size_to_end_of_buffer;
	}
	write_advance(self, data_pointer, len);

	return 0;
}

int nimbus_ring_buffer_read(nimbus_ring_buffer* self, void* data, int len)
{
	int read_octets_left = self->size;
	if (len > read_octets_left) {
		len = read_octets_left;
	}
	int len_to_read = (len > read_octets_left) ? read_octets_left : len;
	u8t* data_pointer = (u8t*) data;

	int size_to_end_of_buffer = self->max_size - self->read_index;
	if (len_to_read > size_to_end_of_buffer) {
		read_advance(self, data_pointer, size_to_end_of_buffer);
		data_pointer += size_to_end_of_buffer;
		len_to_read -= size_to_end_of_buffer;
	}
	read_advance(self, data_pointer, len_to_read);

	return len;
}
