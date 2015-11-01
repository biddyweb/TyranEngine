#include "bit_array.h"

#include <tyranscript/tyran_memory.h>

void nimbus_bit_array_init(nimbus_bit_array* self, struct tyran_memory* memory, int bit_count)
{
	self->max_bits_count = bit_count;
	self->block_count = (self->max_bits_count / 32) + 1;
	self->bits = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, u32t, self->block_count * sizeof(u32t));
}

void nimbus_bit_array_free(nimbus_bit_array* self)
{
	TYRAN_MEMORY_FREE(self->bits);
}

static int find_free_index(nimbus_bit_array* self)
{
	for (int i = 0; i < self->block_count; ++i) {
		u32t d = self->bits[i];
		if (d != 0xffffffff) {
			for (int j = 0; j < 32; ++j) {
				if ((d & 0x1) == 0) {
					return i * 32 + j;
				}
				d >>= 1;
			}
		}
	}
	return -1;
}

static void reserve_index(nimbus_bit_array* self, int index)
{
	int block_index = index / 32;
	u32t mask = 0x1 << (index % 32);
	self->bits[block_index] |= mask;
}

int nimbus_bit_array_reserve_free_index(nimbus_bit_array* self)
{
	int index = find_free_index(self);
	reserve_index(self, index);
	return index;
}

void nimbus_bit_array_delete_index(nimbus_bit_array* self, int index)
{
	int block_index = index / 32;
	u32t mask = ~(0x1 << (index % 32));
	self->bits[block_index] &= mask;
}
