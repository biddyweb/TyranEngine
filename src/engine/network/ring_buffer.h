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

void nimbus_ring_buffer_init(nimbus_ring_buffer* self, struct tyran_memory* memory, int max_length);
int nimbus_ring_buffer_size(nimbus_ring_buffer* self);
void nimbus_ring_buffer_write_pointer(nimbus_ring_buffer* self, u8t** data, int* length);
void nimbus_ring_buffer_read_pointer(nimbus_ring_buffer* self, u8t** data, int* length);
void nimbus_ring_buffer_write(nimbus_ring_buffer* self, const u8t* data, int length);
void nimbus_ring_buffer_write_pointer_advance(nimbus_ring_buffer* self, int count);
void nimbus_ring_buffer_read_pointer_advance(nimbus_ring_buffer* self, int count);
void nimbus_ring_buffer_free(nimbus_ring_buffer* self);
void nimbus_ring_buffer_read(nimbus_ring_buffer* self, u8t* data, int size);

#endif
