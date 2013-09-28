#include "octet_buffer.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>

void nimbus_octet_buffer_init(nimbus_octet_buffer* self, struct tyran_memory* memory, int max_size)
{
	TYRAN_LOG("Buffer init:%d", max_size);
	self->memory = memory;
	self->octets = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, u8t, max_size);
	self->max_size = max_size;
	self->size = 0;
}

void nimbus_octet_buffer_free(nimbus_octet_buffer* self)
{
	TYRAN_MEMORY_FREE(self->octets);
	self->memory = 0;
	self->size = 0;
	self->max_size = 0;
}

void resize_buffer(nimbus_octet_buffer* self, int new_size)
{
	TYRAN_LOG("resize buffer:%d", new_size);
	u8t* replacement_buffer = TYRAN_MEMORY_CALLOC_TYPE_COUNT(self->memory, u8t, new_size);
	tyran_memcpy_type(u8t, replacement_buffer, self->octets, self->size);
	TYRAN_MEMORY_FREE(self->octets);
	self->octets = replacement_buffer;
}

void nimbus_octet_buffer_add(nimbus_octet_buffer* self, const u8t* data, int size)
{
	int total_size = self->size + size;
	if (total_size > self->max_size) {
		resize_buffer(self, total_size * 2);
	}
	tyran_memcpy_type(u8t, self->octets + self->size, data, size);
	self->size += size;
}
