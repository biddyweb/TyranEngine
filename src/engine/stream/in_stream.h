#ifndef nimbus_engine_in_stream_h
#define nimbus_engine_in_stream_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef struct nimbus_in_stream {
	const u8t* buffer;
	const u8t* pointer;
	const u8t* end_buffer;
} nimbus_in_stream;


void nimbus_in_stream_init(nimbus_in_stream* self, const u8t* buffer, int max_octets);
int nimbus_in_stream_read_u8(nimbus_in_stream* self, u8t* data);
int nimbus_in_stream_read_u16(nimbus_in_stream* self, u16t* data);
int nimbus_in_stream_read_u32(nimbus_in_stream* self, u32t* data);
int nimbus_in_stream_read_string(nimbus_in_stream* self, char* data, int max_length);

#endif
