#ifndef nimbus_event_stream_h
#define nimbus_event_stream_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;
struct tyran_string;

typedef u8t nimbus_event_type_id;

typedef struct nimbus_event_stream_header {
	u8t event_type_id;
	u32t event_octet_size;
} nimbus_event_stream_header;

typedef struct nimbus_event_write_stream {
	u8t* buffer;
	u8t* pointer;
	u8t* end_pointer;
	nimbus_event_stream_header* last_header;
} nimbus_event_write_stream;

typedef struct nimbus_event_read_stream {
	const u8t* pointer;
	const u8t* end_pointer;
	nimbus_event_type_id event_type_id;
} nimbus_event_read_stream;


void nimbus_event_write_stream_init(nimbus_event_write_stream* stream, struct tyran_memory* memory, int max_length);
void nimbus_event_write_stream_free(nimbus_event_write_stream* stream);
void nimbus_event_write_stream_clear(nimbus_event_write_stream* self);
int nimbus_event_write_stream_length(nimbus_event_write_stream* self);
void nimbus_event_stream_write_octets(nimbus_event_write_stream* stream, const void* data, int len);
void nimbus_event_stream_write_align(nimbus_event_write_stream* self);
void nimbus_event_stream_write_event_end(nimbus_event_write_stream* self);
void nimbus_event_stream_write_string(nimbus_event_write_stream* self, const struct tyran_string* string);

void nimbus_event_stream_read_init(nimbus_event_read_stream* self, const u8t* pointer, int length);
void nimbus_event_stream_read_octets(nimbus_event_read_stream* stream, u8t* data, int len);
void nimbus_event_stream_read_pointer(nimbus_event_read_stream* stream, const u8t** data, int len);
void nimbus_event_stream_read_align(nimbus_event_read_stream* self);
void nimbus_event_stream_read_skip(nimbus_event_read_stream* self, int length);
void nimbus_event_stream_read_string(nimbus_event_read_stream* self, struct tyran_memory* memory, struct tyran_string* string);

#define nimbus_event_stream_write_type(stream, variable) { nimbus_event_stream_write_align(stream); nimbus_event_stream_write_octets(stream, (const u8t*)(&variable), sizeof(variable)); }
#define nimbus_event_stream_write_event_header(stream, ID) {  nimbus_event_stream_header header; header.event_type_id = ID; header.event_octet_size = 0; nimbus_event_stream_write_type(stream, header); (stream)->last_header = (nimbus_event_stream_header*)((stream)->pointer - sizeof(header)); }
#define nimbus_event_stream_write_event(stream, event_type_id, variable) { nimbus_event_stream_write_event_header(stream, event_type_id); nimbus_event_stream_write_type(stream, variable); nimbus_event_stream_write_event_end(stream); }

#define nimbus_event_stream_read_type(stream, variable) { nimbus_event_stream_read_align(stream); nimbus_event_stream_read_octets(stream, (u8t*)(&variable), sizeof(variable)); }
#define nimbus_event_stream_read_type_pointer(stream, variable, type) { nimbus_event_stream_read_align(stream); variable = (type*) stream->pointer; nimbus_event_stream_read_skip(stream, sizeof(type)); }


#endif
