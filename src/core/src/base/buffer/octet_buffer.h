#ifndef nimbus_octet_buffer_h
#define nimbus_octet_buffer_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef struct nimbus_octet_buffer {
	u8t* octets;
	int size;
	int max_size;
	struct tyran_memory* memory;
} nimbus_octet_buffer;

void nimbus_octet_buffer_init(nimbus_octet_buffer* self, struct tyran_memory* memory, int max_size);
void nimbus_octet_buffer_free(nimbus_octet_buffer* self);
void nimbus_octet_buffer_add(nimbus_octet_buffer* self, const u8t* data, int size);

#endif
