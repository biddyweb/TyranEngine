#ifndef nimbus_bit_array_h
#define nimbus_bit_array_h

struct tyran_memory;

#include <tyranscript/tyran_types.h>

typedef struct nimbus_bit_array {
	u32t* bits;
	int max_bits_count;
	int block_count;
} nimbus_bit_array;

void nimbus_bit_array_init(nimbus_bit_array* self, struct tyran_memory* memory, int bit_count);
int nimbus_bit_array_reserve_free_index(nimbus_bit_array* self);
void nimbus_bit_array_delete_index(nimbus_bit_array* self, int index);

#endif
