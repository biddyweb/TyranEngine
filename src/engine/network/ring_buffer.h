#ifndef nimbus_ring_buffer_h
#define nimbus_ring_buffer_h

typedef struct nimbus_ring_buffer {
	int write_index;
	int read_index;
	u8t* buffer;
	
} nimbus_ring_buffer;

nimbus_ring_buffer* nimbus_ring_buffer_new(tyran_memory* memory, int max_length);
void nimbus_ring_buffer_free(nimbus_ring_buffer* self);
int nimbus_ring_buffer_write(in_buffer* self, const void* data, int len);
int nimbus_ring_buffer_read(nimbus_ring_buffer* self, void* data, int len);

#endif
