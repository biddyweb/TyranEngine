#ifndef nimbus_ring_buffer_h
#define nimbus_ring_buffer_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef struct nimbus_ring_buffer {
	int write_index;
	int read_index;
	u8t* buffer;
	int size;
	int max_size;
	
} nimbus_ring_buffer;

nimbus_ring_buffer* nimbus_ring_buffer_new(struct tyran_memory* memory, int max_length);
void nimbus_ring_buffer_free(nimbus_ring_buffer* self);
int nimbus_ring_buffer_write(nimbus_ring_buffer* self, const void* data, int len);
int nimbus_ring_buffer_read(nimbus_ring_buffer* self, void* data, int len);

#endif
