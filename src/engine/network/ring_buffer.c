#include "ring_buffer.h"

nimbus_ring_buffer* nimbus_ring_buffer_new(tyran_memory* memory, int max_length)
{
	in_buffer* self = TYRAN_CALLOC_TYPE(memory, nimbus_ring_buffer);
	self->buffer = TYRAN_MALLOC(memory, max_length, "circular buffer");
	self->max_size = max_length;
}

void nimbus_ring_buffer_free(nimbus_ring_buffer* self)
{
	TYRAN_FREE(self->buffer);
	TYRAN_FREE(self);
}

static void write_advance(in_buffer* self, const u8t* data_pointer, int size)
{
	tyran_memcpy_octets(self->write_pointer, data_pointer, size);
	self->write_index += size;
	self->write_index %= self->max_size;
	self->size += size;
}

static void read_advance(in_buffer* self, u8t* data_pointer, int size)
{
	tyran_memcpy_octets(data_pointer, self->buffer + self->write_index, size);
	self->read_index += size;
	self->read_index %= self->max_size;
	self->size -= size;
}

int nimbus_ring_buffer_write(in_buffer* self, const void* data, int len)
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
